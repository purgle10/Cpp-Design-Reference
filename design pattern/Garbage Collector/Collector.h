////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Collector.h
//
// summary:	Modified with standard C++11 library
//
// refer:	https://github.com/audulus/collector
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __Collector__
#define __Collector__

#include <set>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>

#include <map>
#include <assert.h>

// Use thread_local ptr to
// access multithread data
static thread_local std::map<void*, std::shared_ptr< void > > thread_specific_ptr_data;

template< typename T >
class thread_specific_ptr
{
public:
	T * get()
	{
		auto it = thread_specific_ptr_data.find(this);
		if (it != thread_specific_ptr_data.end())
			return static_cast< T* >(it->second.get());
		return nullptr;
	}
	T* operator->() { return get(); }
	T& operator* () { return *get(); }
	void reset(T* p = 0) {
		T* cur = get();
		if (cur == p) return;
		delete cur;
	}
};


// Derive from Collectable if you'd like an object
// to be garbage collected.
class Collectable {
  
public:
  
  Collectable() : gcRootCount(0), gcSequence(0) { }
  virtual ~Collectable() { }
  
private:
  
  friend class Collector;

  // Connections as seen by the garbage collector.
  std::vector<Collectable*> gcConnections;
  
  // How many times is this node referenced as
  // a root.
  int gcRootCount;
  
  // Sequence number used to determine if
  // the Collectable has been visited in the
  // current round of GC.
  size_t gcSequence;

};

// The garbage collector. A singleton.
//
// Collector is a mark-sweep garbage collector
// that can be run concurrently in a background
// thread.
//
class Collector {
  
public:
  
  // Get the singleton instance.
  static Collector& GetInstance();
  
  // Add a reference to a root collectable.
  void AddRoot(Collectable*);
  
  // Remove a reference to a root collectable.
  void RemoveRoot(Collectable*);
  
  // Notify the collector of a new reference
  // between Collectables.
  void AddEdge(Collectable*, Collectable*);
  
  // Remove a reference between Collectables.
  void RemoveEdge(Collectable*, Collectable*);
  
  // Process events coming from mutator threads.
  // Calling this is optional but if the mutator threads
  // are generating a lot of changes, yet you don't immedately
  // want to collect, this can be useful to call frequently.
  void ProcessEvents();
  
  // Collect garbage. Make sure you only call
  // this from one thread at a time.
  void Collect();
  
  // Are we in the garbage collector thread?
  bool InGC() {
    if(_inGC.get() == 0) {
      _inGC.reset(new bool(false));
    }
    return *_inGC;
  }
  
private:
  
  Collector();
  
  // An event to update the collectors
  // own representation of the graph.
  struct Event {
    
    enum Type {
      AddRoot,
      RemoveRoot,
      Connect,
      Disconnect
    };
    
    Type type;
    Collectable* a;
    Collectable* b;
    
  };



//  boost::lockfree::queue<Event, boost::lockfree::fixed_sized<true> > _eventQueue;
  std::queue<Event> _eventQueue;
  // All the nodes we've seen.
  std::set<Collectable*> _nodes;
  
  void _PushEvent(const Event& e);
  
  void _ProcessEvents();
  
  // * boost::thread_specific_ptr<bool> _inGC;
  
  thread_specific_ptr<bool> _inGC;



  size_t _sequence;
  
  // Has the graph changed since the
  // last time we collected?
  bool _graphChanged;
  
  // Don't allow more than one thread
  // doing collection.
  // * boost::mutex _mutex;
  std::mutex _mutex;
  
};

// When passing references to Collectables
// on the stack, always use StackPtr.
template<typename T>
class StackPtr {
  
public:
  
  StackPtr() : _ptr(0) { }
  
  explicit StackPtr(T* ptr) : _ptr(ptr) {
    _Retain();
  }
  
  StackPtr(const StackPtr& other) : _ptr(other._ptr) {
    _Retain();
  }
  
  template<class T2>
  StackPtr(const StackPtr<T2>& other) : _ptr(other.Get()) {
    _Retain();
  }
  
  ~StackPtr() {
    _Release();
  }
  
  StackPtr& operator=(const StackPtr& other) {
    if(_ptr != other._ptr) {
      _Release();
      _ptr = other._ptr;
      _Retain();
    }
    return *this;
  }
  
  template<class T2>
  StackPtr& operator=(const StackPtr<T2>& other) {
    if(_ptr != other._ptr) {
      _Release();
      _ptr = other._ptr;
      _Retain();
    }
    return *this;
  }
  
  T* Get() const { return _ptr; }
  
  T& operator*() { return *_ptr; }
  T* operator->() const { assert(_ptr); return _ptr; }
  
  operator T*() { return _ptr; }
  
  operator bool() const { return _ptr != 0; }
  
  bool operator==(const StackPtr& other) const {
    return _ptr == other._ptr;
  }
  
  bool operator!=(const StackPtr& other) const {
    return _ptr != other._ptr;
  }
  
  bool operator<(const StackPtr& other) const {
    return _ptr < other._ptr;
  }
  
  void _Retain() {
    if(_ptr) {
      Collector::GetInstance().AddRoot(_ptr);
    }
  }
  
  void _Release() {
    if(_ptr) {
      Collector::GetInstance().RemoveRoot(_ptr);
    }
  }
  
private:
  
  T* _ptr;
  
}; // class StackPtr

template<class T>
std::ostream& operator<<(std::ostream& out, const StackPtr<T>& p) {
  return out << p._ptr;
}

// When passing references to Collectables
// on the stack, always use StackPtr.
template<typename T>
class HeapPtr {
  
public:

  // Every HeapPtr must have an owner.
  HeapPtr(Collectable* owner) : _owner(owner), _ptr(0) {
    assert(owner);
  }
  
  HeapPtr(Collectable* owner, const StackPtr<T>& other) : _owner(owner), _ptr(other.Get()) {
    assert(owner);
    _Retain();
  }
  
  ~HeapPtr() {

    // If we're not in the GC thread.
    if(! Collector::GetInstance().InGC()) {
      _Release();
    }

  }
  
  HeapPtr& operator=(const HeapPtr& other) {
    assert(_owner == other._owner);
    if(_ptr != other._ptr) {
      _Release();
      _ptr = other._ptr;
      _Retain();
    }
    return *this;
  }
  
  template<class T2>
  HeapPtr& operator=(const StackPtr<T2>& other) {
    if(_ptr != other.Get()) {
      _Release();
      _ptr = other.Get();
      _Retain();
    }
    return *this;
  }
  
  // Create a StackPtr out of this HeapPtr.
  StackPtr<T> GetStackPtr() const { return StackPtr<T>(_ptr); }
  
  operator bool() const { return _ptr != 0; }
  
  bool operator==(const HeapPtr& other) const {
    return _ptr == other._ptr;
  }
  
  bool operator!=(const HeapPtr& other) const {
    return _ptr != other._ptr;
  }
  
  bool operator<(const HeapPtr& other) const {
    return _ptr < other._ptr;
  }
  
private:
  
  void _Retain() {
    if(_ptr) {
      Collector::GetInstance().AddEdge(_owner, _ptr);
    }
  }
  
  void _Release() {
    if(_ptr) {
      Collector::GetInstance().RemoveEdge(_owner, _ptr);
    }
  }
  
  Collectable* _owner;
  T* _ptr;
  
}; // class HeapPtr

#endif /* defined(__Dev__Collector__) */
