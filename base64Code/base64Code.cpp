////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	base64Code.cpp
//
// summary:	base64 encode and decode
////////////////////////////////////////////////////////////////////////////////////////////////////
#include"base64Code.h"
#include<assert.h>
#include<iostream>

const std::string Base64Code::baseString = // base64��������
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string Base64Code::encode(const std::string &s) {
	unsigned char byte3[3]; // �洢3��ԭʼ�ֽ�
	unsigned char byte4[4]; // �洢4��base64�ֽ�
	unsigned group = s.length() / 3; //ÿ3���ֽ�һ��
	unsigned remain = s.length() - 3 * group; // padding
	int pos = 0;
	std::string result;
	result.reserve(4 * group + 4);

	for (int g = 0; g < group; ++g) {
		for (int i = 0; i < 3; ++i) {
			byte3[i] = s[pos++];
		}
		byte4[0] =  (byte3[0] & 0b11111100) >> 2; // ǰ6bits��ֵ��base64��һ���ֽ�
		byte4[1] = ((byte3[0] & 0b00000011) << 4) + ((byte3[1] & 0b11110000) >> 4);// byte3[0]��2bits + byte3[1]ǰ4bits��ɵڶ����ֽ�
		byte4[2] = ((byte3[1] & 0b00001111) << 2) + ((byte3[2] & 0b11000000) >> 6);// byte3[1]��4bits + byte3[2]ǰ4bits��ɵ������ֽ�
		byte4[3] =   byte3[2] & 0b00111111;// byte3[2]��6bits������һ���ֽ�
		for (int i = 0; i < 4; ++i) {
			result += baseString[byte4[i]]; // ����������encode���µ��ַ���
		}
	}
	if (remain > 0) { // ʣ��1-2���ַ���δencode
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
		for (int i = remain + 1; i < 4; ++i) { // ĩλ��'='padding���Զ������
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
	assert(remain == 0); // ��Ӧpadding����
	for (int g = 0; g < group; ++g) {
		for (int i = 0; i < 4; ++i) {
			byte4[i] = baseString.find(s[pos++]); // ���ݱ����ɶ�Ӧ�Ķ�������,��'='�Ҳ�����Ӧʱ������npos����-1��0xff
		}
		byte3[0] = (byte4[0] << 2) + ((byte4[1] & 0b00110000) >> 4); // byte4[0]��6bits+byte4[1]�м�2bits���byte3[0]
		byte3[1] = ((byte4[1] & 0b00001111) << 4) + ((byte4[2] & 0b00111100) >> 2); // byte4[4]��4bits+byte4[2]�м�4bits���byte3[1]
		byte3[2] = ((byte4[2] & 0b00000011) << 6) + byte4[3]; // byte4[2]��2bits+byte4[3]ǰ6bits���byte3[2]
		if (byte4[2] == 255) { // byte4[2]Ϊpadding��,byte3[0]Ϊ��Чbyte
			result += byte3[0];
		}
		else if (byte4[3] == 255) { // byte4[3]Ϊpadding��,byte3[0][1]Ϊ��Чbyte
			result += byte3[0];
			result += byte3[1];
		}
		else { //��paddingλ��
			result += byte3[0];
			result += byte3[1];
			result += byte3[2];
		}
	}
	return result;
}