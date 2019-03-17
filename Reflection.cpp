#include "Reflection.h"
#include <stdlib.h>
#include <sstream>

TypeReflectInfo TypeReflectInfo::InheritanceTable(TypeReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE, 0, 0);
ReflectInfo ReflectInfo::End(0, "", 0); 

int ArrayOffset(const char* id, int accum = 0) 
{
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
	if(const char* idx = strchr(id, '['))
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
	while(*str0 != '\0' && *str1 != '\0' && *str0 == *str1)
	{
		str0 ++;
		str1 ++;
		ret ++;
	}
	return ret;
}

ReflectField ReflectField::Get(const char* field) const 
{
	ReflectField info(0,0);
	if(!reflectable)
		return info;

	ReflectInfoIterator it(*this);
	int n;
	while((info = it.Next()).reflectable)
	{
		n = strcmpidx(info.infos->id, field);
		if(info.infos->id[n] == '\0')
		{
			if(field[n] == '\0')
			{
				if(info.infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_CLASS)
					return info.ClassPtr();
				else
					return info;
			}
			else if(field[n] == '.')
			{
				if(info.infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_CLASS)
					return info.ClassPtr().Get(&field[n + 1]);
			}
			else if(field[n] == '[')
			{
				if(info.infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_VECTOR)
				{
					char* end;
					int idx = strtol(field + n + 1, &end, 10);
					VectorHandler vector_handler = info.GetVectorHandler();
					if(*(end + 1) == '\0')
						return vector_handler->GetElem(idx);
					else
						return vector_handler->GetElem(idx).Get(end + 2);
				}
			}
		}
	}
	return ReflectField(0,0);
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
		case TypeReflectInfo::REFLECT_TYPE_STRING:    As< std::string >()         = str;                            break;
		case TypeReflectInfo::REFLECT_TYPE_POINTER:   ReflectablePtr()->FromReflectString(str);                     break;
		default: break;
	}

	return *this;
}

std::string ReflectField::ToString()const
{
	std::stringstream ss;

	switch(infos->info->reflect_type)
	{
		case TypeReflectInfo::REFLECT_TYPE_BOOL:      ss << As< bool >();               break;
		case TypeReflectInfo::REFLECT_TYPE_CHAR:      ss << (int)As< char >();          break;
		case TypeReflectInfo::REFLECT_TYPE_UCHAR:     ss << (int)As< unsigned char >(); break;
		case TypeReflectInfo::REFLECT_TYPE_SHORT:     ss << As< short >();              break;
		case TypeReflectInfo::REFLECT_TYPE_USHORT:    ss << As< unsigned short >();     break;
		case TypeReflectInfo::REFLECT_TYPE_INT:       ss << As< int >();                break;
		case TypeReflectInfo::REFLECT_TYPE_UINT:      ss << As< unsigned int >();       break;
		case TypeReflectInfo::REFLECT_TYPE_LONG:      ss << As< long >();               break;
		case TypeReflectInfo::REFLECT_TYPE_ULONG:     ss << As< unsigned long >();      break;
		case TypeReflectInfo::REFLECT_TYPE_LONGLONG:  ss << As< long long >();          break;
		case TypeReflectInfo::REFLECT_TYPE_ULONGLONG: ss << As< unsigned long long >(); break;
		case TypeReflectInfo::REFLECT_TYPE_FLOAT:     ss << As< float >();              break;
		case TypeReflectInfo::REFLECT_TYPE_DOUBLE:    ss << As< double >();             break;
		case TypeReflectInfo::REFLECT_TYPE_STRING:    ss << As< std::string >();        break;
		case TypeReflectInfo::REFLECT_TYPE_POINTER:   ss << '\"' << ReflectablePtr()->ToReflectString() << '\"'; break;
		default: break;
	}

	return ss.str();
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
	while(i = strchr(i, '['))
	{
		ret *= atoi(i + 1);
		i = strchr(i, ']') + 1;
	}
	return ret;
}

ReflectField ReflectField::GetElem(int idx) const
{
	if(const char* array_start = strchr(infos->id, '['))
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
		l.push_back(reflectable.ClassPtr());
	}
	else
	{
		l.push_back(reflectable);
	}
}

ReflectField ReflectInfoIterator::Next() 
{
	if(l.empty()) //Done
		return ReflectField(0, 0);

	void* reflectable  = l[l.size() -1].reflectable;
	ReflectInfo* infos = (l[l.size() - 1].infos) ++;
	if(infos->id[0] == '\0') //table end
	{
		l.pop_back();
		return Next();
	}

	switch(infos->info->reflect_type)
	{
		case TypeReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE:
			l.push_back(ReflectField(reflectable, ((ReflectInfosFunc)(infos->ptr))()));
			return Next();

		case TypeReflectInfo::REFLECT_TYPE_PARENT_CLASS: {
			Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
			l.push_back(ReflectField(classObj, (ReflectInfo*)(((ReflectInfosFunc)(infos->info->extra))()->info->extra)));
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
ReflectInfo* DefaultReflectInfo(std::string*)        {DEF_INFO(std::string,        TypeReflectInfo::REFLECT_TYPE_STRING)}