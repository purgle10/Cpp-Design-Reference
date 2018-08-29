#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "Deque.h"
#include "Functional.h"
#include "Vector.h"

namespace MySTL{
	// class of queue
	template<class T, class Container = MySTL::deque<T>>
	class queue{
	public:
		typedef T value_type;
		typedef Container container_type;
		typedef typename Container::reference reference;
		typedef typename Container::const_reference const_reference;
		typedef typename Container::size_type size_type;
	private:
		Container container_;
	public:
		queue(){}
		explicit queue(const container_type& cntr) :container_(cntr){}

		bool empty() const{ return container_.empty(); }
		size_type size() const{ return container_.size(); }
		reference& front(){ return container_.front(); }
		const_reference& front() const{ return container_.front(); }
		reference& back(){ return container_.back(); }
		const_reference& back() const{ return container_.back(); }
		void push(const value_type& val){ container_.push_back(val); }
		void pop(){ container_.pop_front(); }
		void swap(queue& x){ container_.swap(x.container_); }
	public:
		template<class T, class Container>
		friend bool operator== (const queue<T, Container>& lhs, const queue<T, Container>& rhs);
		template <class T, class Container>
		friend bool operator!= (const queue<T, Container>& lhs, const queue<T, Container>& rhs);
		template <class T, class Container>
		friend void swap(queue<T, Container>& x, queue<T, Container>& y);
	};// end of queue

	template <class T, class Container>
	bool operator== (const queue<T, Container>& lhs, const queue<T, Container>& rhs){
		return lhs.container_ == rhs.container_;
	}
	template <class T, class Container>
	bool operator!= (const queue<T, Container>& lhs, const queue<T, Container>& rhs){
		return lhs.container_ != rhs.container_;
	}
	template <class T, class Container>
	void swap(queue<T, Container>& x, queue<T, Container>& y){
		MySTL::swap(x.container_, y.container_);
	}

	// class of priority_queue
	template<class T, class Container = MySTL::vector<T>, 
		class Compare = MySTL::less<typename Container::value_type>>
	class priority_queue{
		typedef T value_type;
		typedef Container container_type;
		typedef typename Container::reference reference;
		typedef typename Container::const_reference const_reference;
		typedef typename Container::size_type size_type;
	private:
		container_type container_;
		Compare compare_;
	public:
		explicit priority_queue(const Compare& comp = Compare(),
			const Container& ctnr = Container())
			: container_(ctnr), compare_(comp){}

		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last,
			const Compare& comp = Compare(),
			const Container& ctnr = Container())
			: container_(ctnr), compare_(comp){
			container_.insert(container_.end(), first, last);
			MySTL::make_heap(container_.begin(), container_.end());
		}

		bool empty()const{
			return container_.empty();
		}

		size_type size() const{
			return container_.size();
		}
		reference top() {
			return container_.front();
		}
		void push(const value_type& val){
			container_.push_back(val);
			MySTL::push_heap(container_.begin(), container_.end(), compare_);
		}
		void pop(){
			MySTL::pop_heap(container_.begin(), container_.end(), compare_);
			container_.pop_back();
		}
		void swap(priority_queue& x){
			MySTL::swap(container_, x.container_);
			MySTL::swap(compare_, x.compare_);
		}
	public:
		template<class T, class Container, class Compare>
		friend void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y);
	};
	template <class T, class Container, class Compare>
	void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y){
		x.swap(y);
	}
}

#endif