#ifndef ENUMREFLECTDATA_H
#define ENUMREFLECTDATA_H

class ReflectField;

class EnumReflectData
{
public:
	const char* str;
	const int value;
	EnumReflectData(const char* str, const int value) : str(str), value(value) {};
};
int EnumIndex(int value, const EnumReflectData* reflectDatas);
const char* EnumStrValue(const ReflectField& reflectable);

#endif