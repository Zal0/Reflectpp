#ifndef JSONTOOLS_H
#define JSONTOOLS_H

#include "Reflection.h"

void Serialize(Reflectable* reflectable, const char* path);
bool Deserialize(Reflectable* reflectable, const char* path);

//Tools
void Serialize(FILE_OUT out, const ReflectField& reflectable);
void Deserialize(FILE_IN in, const ReflectField& reflectable);
void NewLine(FILE_OUT out);
void Comma(FILE_OUT out);
void OpenBracket(FILE_OUT out, char bracket);
void CloseBracket(FILE_OUT out, char bracket);

#endif