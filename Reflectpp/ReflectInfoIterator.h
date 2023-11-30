#ifndef REFLECTINFO_ITERATOR_H
#define REFLECTINFO_ITERATOR_H

#include "ReflectConfig/ReflectConfigTypes.h"
#include "ReflectField.h"
class Reflectable;

class ReflectInfoIterator
{
public:
	VECTOR(ReflectField) l;

private:
	void Init(const ReflectField& reflectable);

public:
	ReflectInfoIterator(Reflectable* reflectable);
	ReflectInfoIterator(const ReflectField& reflectable);
	ReflectField Next();
};

#endif