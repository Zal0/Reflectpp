#ifndef REFLECT_TYPES
#define REFLECT_TYPES

#include "ReflectConfig/ReflectConfigTypes.h"
#include "TypeReflectInfo.h"

TypeReflectInfo* GetTypeReflectInfo(bool*);
TypeReflectInfo* GetTypeReflectInfo(char*);
TypeReflectInfo* GetTypeReflectInfo(unsigned char*);
TypeReflectInfo* GetTypeReflectInfo(short*);
TypeReflectInfo* GetTypeReflectInfo(unsigned short*);
TypeReflectInfo* GetTypeReflectInfo(int*);
TypeReflectInfo* GetTypeReflectInfo(unsigned int*);
TypeReflectInfo* GetTypeReflectInfo(long*);
TypeReflectInfo* GetTypeReflectInfo(unsigned long*);
TypeReflectInfo* GetTypeReflectInfo(long long*);
TypeReflectInfo* GetTypeReflectInfo(unsigned long long*);
TypeReflectInfo* GetTypeReflectInfo(float*);
TypeReflectInfo* GetTypeReflectInfo(double*);
TypeReflectInfo* GetTypeReflectInfo(STRING*);

#endif