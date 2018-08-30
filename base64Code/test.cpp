#include"base64Code.h"
#include<string>
#include<iostream>
int main() {
	std::string s = Base64Code::encode("ABC");
	std::cout << s << std::endl;
	std::string ori = Base64Code::decode(s);
	std::cout << ori << std::endl; // output should be: QUJD
	return 0;
}