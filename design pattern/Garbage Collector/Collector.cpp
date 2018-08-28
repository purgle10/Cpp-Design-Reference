////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Collector.cpp
//
// summary:	Modified a concurrent garbage collector with standard C++11 library
//
// refer:	https://github.com/audulus/collector, implemented with boost library 
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Collector.h"

Collector& Collector::GetInstance() {
	static Collector collector;
	return collector;
}

Collector::Collector() : _eventQueue(std::queue<Event>()), _sequence(0), _graphChanged(false) { }

void Collector::_PushEvent(const Event& e) {
	_mutex.lock();
	_eventQueue.push(e);
	_mutex.unlock();
	//while(!_eventQueue.push(e)) {
	//  std::cout << "Warning: collector queue is full" << std::endl;
	//}

}

void Collector::AddRoot(Collectable* node) {

	Event e;
	e.type = Event::AddRoot;
	e.a = node;

	_PushEvent(e);
}

void Collector::RemoveRoot(Collectable* node) {

	Event e;
	e.type = Event::RemoveRoot;
	e.a = node;

	_PushEvent(e);
}

void Collector::AddEdge(Collectable* a, Collectable* b) {

	Event e;
	e.type = Event::Connect;
	e.a = a;
	e.b = b;

	_PushEvent(e);
}

void Collector::RemoveEdge(Collectable* a, Collectable* b) {

	Event e;
	e.type = Event::Disconnect;
	e.a = a;
	e.b = b;

	_PushEvent(e);
}

void Collector::ProcessEvents() {

	//boost::mutex::scoped_lock lock(_mutex);

	_mutex.lock();

	_ProcessEvents();

	_mutex.unlock();
}

void Collector::_ProcessEvents() {

	Event e;

	while (!_eventQueue.empty()) { // if next pointer is nullptr, return false
		e = _eventQueue.front();
		_eventQueue.pop();

		_graphChanged = true;

		switch (e.type) {
		case Event::AddRoot: {
			_nodes.insert(e.a);
			e.a->gcRootCount++;
		}
			break;
		case Event::RemoveRoot: {

			e.a->gcRootCount--;

			// Root count must be positive.
			assert(e.a->gcRootCount >= 0);
		}
			break;
		case Event::Connect: {

			e.a->gcConnections.push_back(e.b);
		}
			break;
		case Event::Disconnect: {

			std::vector<Collectable*>& adj = e.a->gcConnections;

			auto iter = std::find(adj.begin(), adj.end(), e.b);

			// The connection must exist.
			assert(iter != e.a->gcConnections.end());

			adj.erase(iter);
		}
			break;

		default:
			break;
		}
	}

}

void Collector::Collect() {

	//boost::mutex::scoped_lock lock(_mutex);

	_mutex.lock();

	if (!_inGC.get()) {
		_inGC.reset(new bool);
		_mutex.unlock();
		return; // add
	}

	*_inGC = true; // can't be nullptr

	_ProcessEvents();

	if (_graphChanged) {

		_sequence++;

		std::vector<Collectable*> nodeStack;

		// Traverse starting with roots.
		for (auto node : _nodes) {

			if (node->gcRootCount) {
				nodeStack.push_back(node);
			}
		}

		// Mark.
		while (!nodeStack.empty()) {

			Collectable* node = nodeStack.back();
			nodeStack.pop_back();

			if (node->gcSequence != _sequence) {
				node->gcSequence = _sequence;

				for (auto adj : node->gcConnections) {
					nodeStack.push_back(adj);
				}
			}
		}

		// Sweep.
		std::set<Collectable*> newNodes;

		for (auto node : _nodes) {

			// Not visited.
			if (node->gcSequence != _sequence) {
				delete node;
			}
			else {
				newNodes.insert(node);
			}
		}

		_nodes.swap(newNodes);

		_graphChanged = false;

	}

	*_inGC = false;

	_mutex.unlock();
}

