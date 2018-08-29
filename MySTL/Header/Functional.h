#ifndef _FUNCTIONAL_H_
#define _FUNCTIONAL_H_

namespace MySTL{
	//************ [less] *************
	template<class T>
	struct less{
		typedef T first_arguement_type;
		typedef T second_arguement_type;
		typedef bool result_type;

		result_type operator()(const first_arguement_type& x, const second_arguement_type& y){
			return x < y;
		}
	};

	//************ [equal_to] *************
	template<class T>
	struct equal_to{
		typedef T first_arguement_type;
		typedef T second_arguement_type;
		typedef bool result_type;

		result_type operator()(const first_arguement_type& x, const second_arguement_type& y){
			return x == y;
		}
	};
}

#endif