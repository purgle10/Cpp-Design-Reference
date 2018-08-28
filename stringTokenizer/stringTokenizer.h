///////////////////////////////////////////////////////////////////////////////
// file: stringTokenizer.h
// 
// summary: stringTokenizer for both ascii and chinese text. The default delimiters are space(" "), tab(\t, \v), newline(\n),
//			carriage return(\r), and form feed(\f).
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __stringTokenizer_H__
#define __stringTokenizer_H__

#include <string>
#include <vector>

// default delimiter string (space, tab, newline, carriage return, form feed)
const std::string DEFAULT_DELIMITER = " \t\v\n\r\f";

class stringTokenizer
{
public:
    stringTokenizer();
    stringTokenizer(const std::string& str, const std::string& delimiter=DEFAULT_DELIMITER);
    ~stringTokenizer();

    // set string and delimiter
    void set(const std::string& str, const std::string& delimiter=DEFAULT_DELIMITER);
    void setString(const std::string& str);             // set source string only
    void setDelimiter(const std::string& delimiter);    // set delimiter string only
	// add delimiter
	void addDelimiter(const std::string& delimiter);

    std::string next();                                 // return the next token, return "" if it ends
	std::string nextGB2312();
    std::vector<std::string> split();                   // return array of tokens from current cursor

protected:


private:
    void skipDelimiter();                               // ignore leading delimiters
    bool isDelimiter(char c);                           // check if the current char is delimiter
	bool isChineseCharacter(char c);

    std::string _buffer;                                 // input string
    std::string _token;                                  // output string
    std::string _delimiter;                              // delimiter string
    std::string::const_iterator _currPos;                // string iterator pointing the current position

};

#endif // end of __stringTokenizer_H__
