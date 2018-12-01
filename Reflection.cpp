#include "Reflection.h"

ReflectInfoIterator::Reflectable_Info ReflectInfoIterator::Reflectable_Info::Get(const char* field) 
{
	return Reflectable::Get(field, reflectable, infos);
}