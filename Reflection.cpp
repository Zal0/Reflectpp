#include "Reflection.h"
#include <stdlib.h>
#include <sstream>

ReflectInfo ReflectInfo::End(REFLECT_TYPE_INT, "", 0); 

ReflectField::ReflectField(Reflectable* reflectable)
{
	//Instead of directly point to reflectable infos, create a dummy table (simplifies things, see PrintReflectable or Serialize)
	classDummyInfos[0] = ReflectInfo(ReflectInfo::REFLECT_TYPE_CLASS, "", 0, (PTR)(reflectable->ReflectInfosF()()));
	classDummyInfos[1] = ReflectInfo::End;

	this->reflectable = reflectable->This();
	this->infos = classDummyInfos;
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
				if(info.infos->reflect_type == ReflectInfo::REFLECT_TYPE_CLASS)
					return info.ClassPtr();
				else
					return info;
			}
			else if(field[n] == '.')
			{
				if(info.infos->reflect_type == ReflectInfo::REFLECT_TYPE_CLASS)
					return info.ClassPtr().Get(&field[n + 1]);
			}
			else if(field[n] == '[')
			{
				if(info.infos->reflect_type == ReflectInfo::REFLECT_TYPE_VECTOR)
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
	switch(infos->reflect_type)
	{
		case ReflectInfo::REFLECT_TYPE_BOOL:      As< bool >()                = str[0] == '0' ? false : true;   break;
		case ReflectInfo::REFLECT_TYPE_CHAR:      As< char >()                = (char)atoi(str);                break;
		case ReflectInfo::REFLECT_TYPE_UCHAR:     As< unsigned char >()       = (unsigned char)atoi(str);       break;
		case ReflectInfo::REFLECT_TYPE_SHORT:     As< short >()               = (short)atoi(str);               break;
		case ReflectInfo::REFLECT_TYPE_USHORT:    As< unsigned short >()      = (unsigned short)atoi(str);      break;
		case ReflectInfo::REFLECT_TYPE_INT:       As< int >()                 = (int)atoi(str);                 break;
		case ReflectInfo::REFLECT_TYPE_UINT:      As< unsigned int >()        = (unsigned int)atoi(str);        break;
		case ReflectInfo::REFLECT_TYPE_LONG:      As< long >()                = (long)atol(str);                break;
		case ReflectInfo::REFLECT_TYPE_ULONG:     As< unsigned long >()       = (unsigned long)atol(str);       break;
		case ReflectInfo::REFLECT_TYPE_LONGLONG:  As< long long >()           = (long long)atoll(str);          break;
		case ReflectInfo::REFLECT_TYPE_ULONGLONG: As< unsigned long long >()  = (unsigned long long)atoll(str); break;
		case ReflectInfo::REFLECT_TYPE_FLOAT:     As< float >()               = (float)atof(str);               break;
		case ReflectInfo::REFLECT_TYPE_DOUBLE:    As< double >()              = (double)atof(str);              break;
		case ReflectInfo::REFLECT_TYPE_STRING:    As< std::string >()         = str;                            break;
		default: break;
	}

	return *this;
}

std::string ReflectField::ToString()const
{
	std::stringstream ss;

	switch(infos->reflect_type)
	{
		case ReflectInfo::REFLECT_TYPE_BOOL:      ss << As< bool >();               break;
		case ReflectInfo::REFLECT_TYPE_CHAR:      ss << As< char >();               break;
		case ReflectInfo::REFLECT_TYPE_UCHAR:     ss << As< unsigned char >();      break;
		case ReflectInfo::REFLECT_TYPE_SHORT:     ss << As< short >();              break;
		case ReflectInfo::REFLECT_TYPE_USHORT:    ss << As< unsigned short >();     break;
		case ReflectInfo::REFLECT_TYPE_INT:       ss << As< int >();                break;
		case ReflectInfo::REFLECT_TYPE_UINT:      ss << As< unsigned int >();       break;
		case ReflectInfo::REFLECT_TYPE_LONG:      ss << As< long >();               break;
		case ReflectInfo::REFLECT_TYPE_ULONG:     ss << As< unsigned long >();      break;
		case ReflectInfo::REFLECT_TYPE_LONGLONG:  ss << As< long long >();          break;
		case ReflectInfo::REFLECT_TYPE_ULONGLONG: ss << As< unsigned long long >(); break;
		case ReflectInfo::REFLECT_TYPE_FLOAT:     ss << As< float >();              break;
		case ReflectInfo::REFLECT_TYPE_DOUBLE:    ss << As< double >();             break;
		case ReflectInfo::REFLECT_TYPE_STRING:    ss << As< std::string >();        break;
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
		return ((VectorHandlerFunc)infos->extra)(REFLECT_PTR(void, reflectable, infos->ptr));
	else
		return VectorHandler(new NullVectorHandler());
}

ReflectInfoIterator::ReflectInfoIterator(const ReflectField& reflectable)
{
	//Ignore the reflectable ReflectInfo::REFLECT_TYPE_CLASS that was added during casting
	if(reflectable.infos->id[0] == '\0' && reflectable.infos->reflect_type == ReflectInfo::REFLECT_TYPE_CLASS)
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

	switch(infos->reflect_type)
	{
		case ReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE:
			l.push_back(ReflectField(reflectable, ((ReflectInfosFunc)(infos->ptr))()));
			return Next();

		case ReflectInfo::REFLECT_TYPE_PARENT_CLASS: {
			Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
			l.push_back(ReflectField(classObj, ((ReflectInfosFunc)(infos->extra))()));
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

ReflectInfo::ReflectType ReflectTypeBySize(int size) {
	switch (size)
	{
		case 1:  return ReflectInfo::REFLECT_TYPE_CHAR;
		case 2:  return ReflectInfo::REFLECT_TYPE_SHORT;
		case 8:  return ReflectInfo::REFLECT_TYPE_LONGLONG; 
		default: return ReflectInfo::REFLECT_TYPE_INT;
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
	} while((reflectDatas ++)->value != value);

	return 0; //Not found
}

const char* EnumStrValue(int value, const EnumReflectData* reflectDatas)
{
	return reflectDatas[EnumIndex(value, reflectDatas)].str;
}

ReflectInfo* DefaultReflectInfo(bool*)               {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_BOOL,       "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(char*)               {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_CHAR,       "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(unsigned char*)      {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_UCHAR,      "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(short*)              {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_SHORT,      "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(unsigned short*)     {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_USHORT,     "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(int*)                {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_INT,        "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(unsigned int*)       {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_UINT,       "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(long*)               {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_LONG,       "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(unsigned long*)      {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_ULONG,      "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(long long*)          {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_LONGLONG,   "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(unsigned long long*) {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_ULONGLONG,  "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(float*)              {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_FLOAT,      "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(double*)             {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_DOUBLE,     "", 0); return &ret;}
ReflectInfo* DefaultReflectInfo(std::string*)        {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_STRING,     "", 0); return &ret;}