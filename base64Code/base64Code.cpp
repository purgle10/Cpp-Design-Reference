////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	base64Code.cpp
//
// summary:	base64 encode and decode
////////////////////////////////////////////////////////////////////////////////////////////////////
#include"base64Code.h"
#include<assert.h>
#include<iostream>

const std::string Base64Code::baseString = // base64编码索引
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string Base64Code::encode(const std::string &s) {
	unsigned char byte3[3]; // 存储3个原始字节
	unsigned char byte4[4]; // 存储4个base64字节
	unsigned group = s.length() / 3; //每3个字节一组
	unsigned remain = s.length() - 3 * group; // padding
	int pos = 0;
	std::string result;
	result.reserve(4 * group + 4);

	for (int g = 0; g < group; ++g) {
		for (int i = 0; i < 3; ++i) {
			byte3[i] = s[pos++];
		}
		byte4[0] =  (byte3[0] & 0b11111100) >> 2; // 前6bits赋值给base64第一个字节
		byte4[1] = ((byte3[0] & 0b00000011) << 4) + ((byte3[1] & 0b11110000) >> 4);// byte3[0]后2bits + byte3[1]前4bits组成第二个字节
		byte4[2] = ((byte3[1] & 0b00001111) << 2) + ((byte3[2] & 0b11000000) >> 6);// byte3[1]后4bits + byte3[2]前4bits组成第三个字节
		byte4[3] =   byte3[2] & 0b00111111;// byte3[2]后6bits组成最后一个字节
		for (int i = 0; i < 4; ++i) {
			result += baseString[byte4[i]]; // 根据索引表，encode成新的字符串
		}
	}
	if (remain > 0) { // 剩下1-2个字符还未encode
		for (int i = 0; i < remain; ++i) {
			byte3[i] = s[pos++];
		}
		for (int i = remain; i < 3; ++i) {
			byte3[i] = 0;
		}
		byte4[0] =  (byte3[0] & 0b11111100) >> 2;
		byte4[1] = ((byte3[0] & 0b00000011) << 4) + ((byte3[1] & 0b11110000) >> 4);
		byte4[2] = ((byte3[1] & 0b00001111) << 2) + ((byte3[2] & 0b11000000) >> 6);
		byte4[3] =   byte3[2] & 0b00111111;
		for (int i = 0; i < remain + 1; ++i) {
			result += baseString[byte4[i]];
		}
		for (int i = remain + 1; i < 4; ++i) { // 末位加'='padding，自定义规则
			result.push_back('=');
		}
	}
	return result;
}

std::string Base64Code::decode(const std::string& s) {
	unsigned char byte3[3];
	unsigned char byte4[4];
	unsigned group = s.length() / 4;
	const unsigned remain = s.length() - 4 * group;
	int pos = 0;
	std::string result;
	result.reserve(3 * group);
	assert(remain == 0); // 对应padding部分
	for (int g = 0; g < group; ++g) {
		for (int i = 0; i < 4; ++i) {
			byte4[i] = baseString.find(s[pos++]); // 根据表生成对应的二进制码,当'='找不到对应时，返回npos，即-1，0xff
		}
		byte3[0] = (byte4[0] << 2) + ((byte4[1] & 0b00110000) >> 4); // byte4[0]后6bits+byte4[1]中间2bits组成byte3[0]
		byte3[1] = ((byte4[1] & 0b00001111) << 4) + ((byte4[2] & 0b00111100) >> 2); // byte4[4]后4bits+byte4[2]中间4bits组成byte3[1]
		byte3[2] = ((byte4[2] & 0b00000011) << 6) + byte4[3]; // byte4[2]后2bits+byte4[3]前6bits组成byte3[2]
		if (byte4[2] == 255) { // byte4[2]为padding码,byte3[0]为有效byte
			result += byte3[0];
		}
		else if (byte4[3] == 255) { // byte4[3]为padding码,byte3[0][1]为有效byte
			result += byte3[0];
			result += byte3[1];
		}
		else { //非padding位置
			result += byte3[0];
			result += byte3[1];
			result += byte3[2];
		}
	}
	return result;
}