#ifndef JSONTOOLS_H
#define JSONTOOLS_H

#include "Reflection.h"

void Serialize(Reflectable* reflectable, char* path);
void Deserialize(Reflectable* reflectable, char* path);
#endif