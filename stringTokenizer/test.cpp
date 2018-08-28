#include "stringTokenizer.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	string ori_str("this is an example 可以支持中文吗？ yes");
	stringTokenizer str(ori_str);
	string token;
	
	while ((token = str.nextGB2312()) != "") {
		cout << token << endl;
	}

	return 0;
}