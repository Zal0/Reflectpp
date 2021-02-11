#ifndef JSONTOOLS_H
#define JSONTOOLS_H

#include "Reflection.h"

void Serialize(Reflectable* reflectable, const char* path);
bool Deserialize(Reflectable* reflectable, const char* path);
#endif