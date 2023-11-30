#ifndef REFLECTINFO_H
#define REFLECTINFO_H

#include "TypeReflectInfo.h"

class ReflectInfo
{
public:
	TypeReflectInfo* info;
	const char* id;
	PTR offset;

public:
	ReflectInfo() {};
	ReflectInfo(TypeReflectInfo* info, const char* id, PTR offset);
};

#endif