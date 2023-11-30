#ifndef JSONTOOLS_H
#define JSONTOOLS_H

#include "Reflection.h"

class PeekStream
{
private:
	char c; //The next token available (already read, is cached here)
	int depth;
	FILE_IN stream;

	void Get();
	bool IsWhiteSpace();
	bool IsSpecialChar();

public:
	STRING buffer;

	PeekStream(FILE_IN stream);

	STRING& NextToken();
	STRING& ReadBlock(char open_char, char end_char);
	void SkipBlock(char open_char, char end_char);
};

void Serialize(Reflectable* reflectable, const char* path);
bool Deserialize(Reflectable* reflectable, const char* path);

//Tools
void Serialize(Reflectable* reflectable, FILE_OUT out);
void Deserialize(Reflectable* reflectable, FILE_IN in);
void Deserialize(ReflectField& reflectable, PeekStream& in);
void NewLine(FILE_OUT out);
void Comma(FILE_OUT out);
void OpenBracket(FILE_OUT out, char bracket);
void CloseBracket(FILE_OUT out, char bracket);

#endif