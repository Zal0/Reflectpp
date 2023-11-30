#ifndef TYPEREFLECTINFO_H
#define TYPEREFLECTINFO_H

#include "ReflectTools.h"
#include "ReflectConfig/ReflectConfigTypes.h"

class TypeReflectInfo
{
public:
	Reflectpp::Type reflect_type;
	unsigned int size;
	PTR extra;

public:
	TypeReflectInfo() {};
	TypeReflectInfo(Reflectpp::Type reflect_type, unsigned int size, PTR extra) : reflect_type(reflect_type), size(size), extra(extra) {}
	static TypeReflectInfo InheritanceTable;
};

#endif