///////////////////////////////////////////////////////////////////////////////
// 
// file: stringTokenizer.cpp
//
// summary: stringTokenizer (C++ string version) support for ascii and chinese text
//
///////////////////////////////////////////////////////////////////////////////

#include "stringTokenizer.h"

// default constructor and user defined constructor
stringTokenizer::stringTokenizer() : _buffer(""), _token(""), _delimiter(DEFAULT_DELIMITER){
    _currPos = _buffer.begin();
}

stringTokenizer::stringTokenizer(const std::string& str, const std::string& delimiter) : _buffer(str), _token(""), _delimiter(delimiter){
    _currPos = _buffer.begin();
}



stringTokenizer::~stringTokenizer(){
}


// reset string _buffer, _delimiter and _currPos(the currsor position)
void stringTokenizer::set(const std::string& str, const std::string& delimiter){
    _buffer = str;
    _delimiter = delimiter;
    _currPos = _buffer.begin();
}

void stringTokenizer::setString(const std::string& str){
    _buffer = str;
    _currPos = _buffer.begin();
}

void stringTokenizer::setDelimiter(const std::string& delimiter){
    _delimiter = delimiter;
}

void stringTokenizer::addDelimiter(const std::string& delimiter) {
	_delimiter = _delimiter + delimiter;
}

// return the next token
std::string stringTokenizer::next() {
    if(_buffer.size() <= 0) return "";           // skip if _buffer is empty

    _token.clear();                              // reset _token string

    skipDelimiter();                      // skip leading _delimiters

    // append each char to _token string until it meets _delimiter
    while(_currPos != _buffer.end() && !isDelimiter(*_currPos))
    {
        _token += *_currPos;
        ++_currPos;
    }
    return _token;
}

std::string stringTokenizer::nextGB2312() {
	if (_buffer.size() <= 0) return "";

	_token.clear();

	skipDelimiter();
	
	while (_currPos != _buffer.end() && !isDelimiter(*_currPos)) {		
		if (isChineseCharacter(*_currPos)) {
			if (!_token.empty()) // a text before a chinese character, break
				break;
			_token += *_currPos++; // move forward two bytes
			_token += *_currPos++;
			break; // combine a chinese character, break;
		}
		else {
			_token += *_currPos;
			++_currPos;
		}
	}
	return _token;
}

// skip ang leading _delimiters
void stringTokenizer::skipDelimiter() {
    while(_currPos != _buffer.end() && isDelimiter(*_currPos))
        ++_currPos;
}


// return true if the current character is _delimiter
bool stringTokenizer::isDelimiter(char c) {
    return (_delimiter.find(c) != std::string::npos);
}

bool stringTokenizer::isChineseCharacter(char c) {
	return (*_currPos > char(0xa1)) && (*_currPos < char(0xfe));
}


// split the input string into multiple _tokens
// This function scans _tokens from the current cursor position.
std::vector<std::string> stringTokenizer::split() {
    std::vector<std::string> _tokens;
    std::string _token;
    while((_token = next()) != "")
    {
        _tokens.push_back(_token);
    }

    return _tokens;
}
