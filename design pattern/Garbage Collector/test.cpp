////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	test.cpp
//
// summary:	this is an example of gc, for steady use, plz modified for your project
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Collector.h"
#include <Windows.h>

class Node : public Collectable {

public:

	// Pass a pointer to a Node via a StackPtr, since it's on the stack.
	void AddEdge(const StackPtr<Node> &node) {

		// Store pointers in the heap as HeapPtrs.
		_edges.push_back(HeapPtr<Node>(this, node));
	}

private:

	std::vector< HeapPtr< Node > > _edges;
};

int main() {
	StackPtr<Node> node(new Node); // The collector uses the normal new and delete allocators.

	int n = 0;
	while (false) {

		// Update frequently to keep the event
		// queue from filling up.		
		Collector::GetInstance().ProcessEvents();

		// Collect occasionally.
		if ((++n % 100) == 0) {
			Collector::GetInstance().Collect();
		}

		// Sleep for .01 seconds.
	//	boost::this_thread::sleep(boost::posix_time::millisec(10));
		Sleep(10);
		if (n > 1000)
			break;
	}

	return 0;
}