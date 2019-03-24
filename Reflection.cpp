#include "Reflection.h"
#include <stdlib.h>
#include <sstream>
#include <string.h>

TypeReflectInfo TypeReflectInfo::InheritanceTable(TypeReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE, 0, 0);
ReflectInfo ReflectInfo::End(0, "", 0); 

int ArrayOffset(const char* id, int accum = 0) 
{
	//offset in [2][3][4] is calculated as
	//2
	//2 * 3 + 3 = (2 + 1) * 3 = 9
	//9 * 4 + 4 = (9 + 1) * 4 = 20
 	if(const char* idx = strchr(id, '['))
	{
		int offset = (accum + 1) * atoi(idx + 1);
		return ArrayOffset(strchr(idx, ']') + 1, offset);
	}
	return accum;
}

ReflectInfo::ReflectInfo(TypeReflectInfo* info, const char* id, PTR ptr) : info(info), id(id), ptr(ptr) {
	//Offset address in arrays have been calculated based on the last element, fix it
	//In case of bidimensional arrays is a bit more complex: [2][3][4] offste is 2*3*4 + 3*4 + 4
	if(strchr(id, '['))
	{
		this->ptr -= ArrayOffset(id) * info->size;
	}
}

ReflectField::ReflectField(Reflectable* reflectable)
{
	//Instead of directly point to reflectable infos, create a dummy table (simplifies things, see PrintReflectable or Serialize)
	classDummyInfos[0] = *reflectable->DefaultReflectInfoF()();
	classDummyInfos[1] = ReflectInfo::End;

	this->reflectable = reflectable->This();
	this->infos = classDummyInfos;
}

ReflectField::ReflectField(void* reflectable, ReflectInfo* infos) : 
	reflectable(reflectable), 
	infos(infos) 
{
	classDummyInfos[0] = ReflectInfo::End;
	classDummyInfos[1] = ReflectInfo::End;
}

ReflectField::ReflectField(const ReflectField& r) :
	reflectable(r.reflectable)
,	infos(r.infos) 
{
	if(r.infos == r.classDummyInfos)
	{
		classDummyInfos[0] = r.classDummyInfos[0];
		classDummyInfos[1] = r.classDummyInfos[1];
		infos = classDummyInfos;
	}
}

static int strcmpidx(const char* str0, const char* str1)
{
	int ret = 0;
	while(*str0 != '\0' && *str0 != '[' && *str1 != '\0' && *str0 == *str1)
	{
		str0 ++;
		str1 ++;
		ret ++;
	}
	return ret;
}

ReflectField ReflectField::Get(const char* field) const 
{
	if(field[0] == '\0')
	{
		return *this;
	}
	else if(field[0] == '.') 
	{
		return Get(field + 1);
	}
	else if(field[0] == '[' && IsArray())
	{
		char* end;
		int idx = strtol(field + 1, &end, 10);
		return GetElem(idx).Get(end + 1);
	}
	else
	{
		ReflectField info(0,0);
		ReflectInfoIterator it(ClassPtr());
		int n;
		while((info = it.Next()).reflectable)
		{
			n = strcmpidx(info.infos->id, field);
			if((info.infos->id[n] == '\0' || info.infos->id[n] == '[') && (field[n] == '\0' || field[n] == '[' || field[n] == '.'))
			{
				return info.Get(&field[n]);
			}
		}
		return info;
	}
}

ReflectField& ReflectField::operator=(const char* str)
{
	switch(infos->info->reflect_type)
	{
		case TypeReflectInfo::REFLECT_TYPE_BOOL:      As< bool >()                = str[0] == '0' ? false : true;   break;
		case TypeReflectInfo::REFLECT_TYPE_CHAR:      As< char >()                = (char)atoi(str);                break;
		case TypeReflectInfo::REFLECT_TYPE_UCHAR:     As< unsigned char >()       = (unsigned char)atoi(str);       break;
		case TypeReflectInfo::REFLECT_TYPE_SHORT:     As< short >()               = (short)atoi(str);               break;
		case TypeReflectInfo::REFLECT_TYPE_USHORT:    As< unsigned short >()      = (unsigned short)atoi(str);      break;
		case TypeReflectInfo::REFLECT_TYPE_INT:       As< int >()                 = (int)atoi(str);                 break;
		case TypeReflectInfo::REFLECT_TYPE_UINT:      As< unsigned int >()        = (unsigned int)atoi(str);        break;
		case TypeReflectInfo::REFLECT_TYPE_LONG:      As< long >()                = (long)atol(str);                break;
		case TypeReflectInfo::REFLECT_TYPE_ULONG:     As< unsigned long >()       = (unsigned long)atol(str);       break;
		case TypeReflectInfo::REFLECT_TYPE_LONGLONG:  As< long long >()           = (long long)atoll(str);          break;
		case TypeReflectInfo::REFLECT_TYPE_ULONGLONG: As< unsigned long long >()  = (unsigned long long)atoll(str); break;
		case TypeReflectInfo::REFLECT_TYPE_FLOAT:     As< float >()               = (float)atof(str);               break;
		case TypeReflectInfo::REFLECT_TYPE_DOUBLE:    As< double >()              = (double)atof(str);              break;
		case TypeReflectInfo::REFLECT_TYPE_STRING:    As< STRING >()              = str;                            break;
		case TypeReflectInfo::REFLECT_TYPE_POINTER:   ReflectablePtr()->FromReflectString(str);                     break;
		default: break;
	}

	return *this;
}

#define TO_STRING(STR, TYPE) snprintf(buff, BUFF_SIZE, STR, As< TYPE >());
STRING ReflectField::ToString()const
{
	static const int BUFF_SIZE = 50;
	static char buff[BUFF_SIZE];
	
	switch(infos->info->reflect_type)
	{
		case TypeReflectInfo::REFLECT_TYPE_BOOL:      TO_STRING("%d", bool);                 break;
		case TypeReflectInfo::REFLECT_TYPE_CHAR:      TO_STRING("%c", char);                 break;
		case TypeReflectInfo::REFLECT_TYPE_UCHAR:     TO_STRING("%u", unsigned char);        break;
		case TypeReflectInfo::REFLECT_TYPE_SHORT:     TO_STRING("%hd", short);               break;
		case TypeReflectInfo::REFLECT_TYPE_USHORT:    TO_STRING("%hu", unsigned short);      break;
		case TypeReflectInfo::REFLECT_TYPE_INT:       TO_STRING("%d", int);                  break;
		case TypeReflectInfo::REFLECT_TYPE_UINT:      TO_STRING("%u", unsigned int);         break;
		case TypeReflectInfo::REFLECT_TYPE_LONG:      TO_STRING("%ld", long);                break;
		case TypeReflectInfo::REFLECT_TYPE_ULONG:     TO_STRING("%lu", unsigned long);       break;
		case TypeReflectInfo::REFLECT_TYPE_LONGLONG:  TO_STRING("%lld", long long);          break;
		case TypeReflectInfo::REFLECT_TYPE_ULONGLONG: TO_STRING("%llu", unsigned long long); break;
		case TypeReflectInfo::REFLECT_TYPE_FLOAT:     TO_STRING("%f", float);                break;
		case TypeReflectInfo::REFLECT_TYPE_DOUBLE:    TO_STRING("%lf", double);              break;
		case TypeReflectInfo::REFLECT_TYPE_STRING:    return As< STRING >();
		case TypeReflectInfo::REFLECT_TYPE_POINTER:   STRING() + '\"' + ReflectablePtr()->ToReflectString() + '\"'; break;
		default: break;
	}

	return STRING(buff);
}

class NullVectorHandler : public VectorHandlerI
{
public:
	virtual int GetNumElems(){return 0;}
	virtual void Push() {}
	virtual void Pop() {}
	virtual void Clear() {}

protected:
	virtual void* GetElemPtr(int idx) {return 0;}
	virtual ReflectInfo* GetItemsReflectInfos() {return 0;}
};

VectorHandler ReflectField::GetVectorHandler() const
{
	if(reflectable)
		return ((VectorHandlerFunc)infos->info->extra)(REFLECT_PTR(void, reflectable, infos->ptr));
	else
		return VectorHandler(new NullVectorHandler());
}

bool ReflectField::IsArray() const
{
	return infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_VECTOR || strchr(infos->id, '[');
}

int ReflectField::GetNumElems() const
{
	if(const char* idx = strchr(infos->id, '['))
	{
		return atoi(++ idx);
	}
	else if(infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_VECTOR) 
	{
		VectorHandler vector_handler = GetVectorHandler();
		return vector_handler->GetNumElems();
	}
	return 1;
}

int ReflectField::GetArrayElemSize() const
{
	int ret = infos->info->size;
	const char* i = strchr(infos->id, ']');
	while((i = strchr(i, '[')))
	{
		ret *= atoi(i + 1);
		i = strchr(i, ']') + 1;
	}
	return ret;
}

ReflectField ReflectField::GetElem(int idx) const
{
	if(strchr(infos->id, '['))
	{
		const char* array_end = strchr(infos->id, ']');

		ReflectField ret(((unsigned char*)reflectable) + infos->ptr + idx * GetArrayElemSize(), infos);
		ret.infos = &ret.classDummyInfos[0];
		*ret.infos = *infos;
		ret.infos->id = array_end + 1;
		ret.infos->ptr = 0;
		return ret;
	}
	else if(infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_VECTOR) 
	{
		VectorHandler vector_handler = GetVectorHandler();
		return vector_handler->GetElem(idx);
	}
	return ReflectField(0, 0);
}

ReflectInfoIterator::ReflectInfoIterator(const ReflectField& reflectable)
{
	//Ignore the reflectable TypeReflectInfo::REFLECT_TYPE_CLASS that was added during casting
	if(reflectable.infos->id[0] == '\0' && reflectable.infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_CLASS)
	{
		VECTOR_PUSH(l, reflectable.ClassPtr());
	}
	else
	{
		VECTOR_PUSH(l, reflectable);
	}
}

ReflectField ReflectInfoIterator::Next() 
{
	if(VECTOR_IS_EMPTY(l)) //Done
		return ReflectField(0, 0);

	void* reflectable  = VECTOR_GET(l, VECTOR_SIZE(l) - 1).reflectable;
	ReflectInfo* infos = (VECTOR_GET(l, VECTOR_SIZE(l) - 1).infos) ++;
	if(infos->id[0] == '\0') //table end
	{
		VECTOR_POP(l);
		return Next();
	}

	switch(infos->info->reflect_type)
	{
		case TypeReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE:
			VECTOR_PUSH(l, ReflectField(reflectable, ((ReflectInfosFunc)(infos->ptr))()));
			return Next();

		case TypeReflectInfo::REFLECT_TYPE_PARENT_CLASS: {
			Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
			VECTOR_PUSH(l, ReflectField(classObj, (ReflectInfo*)(((ReflectInfosFunc)(infos->info->extra))()->info->extra)));
			return Next();
		}

		default:
			return ReflectField(reflectable, infos);
	}
}

ReflectField Reflectable::Get(const char* field)
{
	return ReflectField(this).Get(field);
}

TypeReflectInfo::ReflectType ReflectTypeBySize(int size) {
	switch (size)
	{
		case 1:  return TypeReflectInfo::REFLECT_TYPE_CHAR;
		case 2:  return TypeReflectInfo::REFLECT_TYPE_SHORT;
		case 8:  return TypeReflectInfo::REFLECT_TYPE_LONGLONG; 
		default: return TypeReflectInfo::REFLECT_TYPE_INT;
	}
}

int EnumIndex(int value, const EnumReflectData* reflectDatas)
{
	int idx = 0;
	do
	{
		if(reflectDatas->value == value)
		{
			return idx;
		}
		idx ++;
		reflectDatas ++;
	} while(reflectDatas->str[0] != '?' && (reflectDatas)->value != value);

	return idx; //Not found
}

const char* EnumStrValue(const ReflectField& reflectable)
{
	const EnumReflectData* reflectDatas = reflectable.EnumData();
	int index;
	switch(reflectable.infos->info->reflect_type) 
	{
		case TypeReflectInfo::REFLECT_TYPE_CHAR:  index = (int)reflectable.As< char >(); break;
		case TypeReflectInfo::REFLECT_TYPE_SHORT: index = (int)reflectable.As< short >(); break;
		default: index = reflectable.As< int >(); break;
	}

	return reflectDatas[EnumIndex(index, reflectDatas)].str;
}

#define DEF_INFO(TYPE, RTYPE) static TypeReflectInfo t_info(RTYPE, sizeof(TYPE), 0); static ReflectInfo ret(&t_info, "", 0); return &ret;

ReflectInfo* DefaultReflectInfo(bool*)               {DEF_INFO(bool,               TypeReflectInfo::REFLECT_TYPE_BOOL)}
ReflectInfo* DefaultReflectInfo(char*)               {DEF_INFO(char,               TypeReflectInfo::REFLECT_TYPE_CHAR)}
ReflectInfo* DefaultReflectInfo(unsigned char*)      {DEF_INFO(unsigned char,      TypeReflectInfo::REFLECT_TYPE_UCHAR)}
ReflectInfo* DefaultReflectInfo(short*)              {DEF_INFO(short,              TypeReflectInfo::REFLECT_TYPE_SHORT)}
ReflectInfo* DefaultReflectInfo(unsigned short*)     {DEF_INFO(unsigned short,     TypeReflectInfo::REFLECT_TYPE_USHORT)}
ReflectInfo* DefaultReflectInfo(int*)                {DEF_INFO(int,                TypeReflectInfo::REFLECT_TYPE_INT)}
ReflectInfo* DefaultReflectInfo(unsigned int*)       {DEF_INFO(unsigned int,       TypeReflectInfo::REFLECT_TYPE_UINT)}
ReflectInfo* DefaultReflectInfo(long*)               {DEF_INFO(long,               TypeReflectInfo::REFLECT_TYPE_LONG)}
ReflectInfo* DefaultReflectInfo(unsigned long*)      {DEF_INFO(unsigned long,      TypeReflectInfo::REFLECT_TYPE_ULONG)}
ReflectInfo* DefaultReflectInfo(long long*)          {DEF_INFO(long long,          TypeReflectInfo::REFLECT_TYPE_LONGLONG)}
ReflectInfo* DefaultReflectInfo(unsigned long long*) {DEF_INFO(unsigned long long, TypeReflectInfo::REFLECT_TYPE_ULONGLONG)}
ReflectInfo* DefaultReflectInfo(float*)              {DEF_INFO(float,              TypeReflectInfo::REFLECT_TYPE_FLOAT)}
ReflectInfo* DefaultReflectInfo(double*)             {DEF_INFO(double,             TypeReflectInfo::REFLECT_TYPE_DOUBLE)}
ReflectInfo* DefaultReflectInfo(STRING*)             {DEF_INFO(STRING,             TypeReflectInfo::REFLECT_TYPE_STRING)}