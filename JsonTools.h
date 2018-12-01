#ifndef JSONTOOLS_H
#define JSONTOOLS_H

#include "Reflection.h"

void Serialize(void* reflectable, char* path);
void Deserialize(void* reflectable, char* path);
#endif