#include "Reflection.h"
#include <stdlib.h>

ReflectInfo ReflectInfo::End(REFLECT_TYPE_INT, "", 0); 

ReflectField::ReflectField(Reflectable* reflectable)
{
	//Instead of directly point to reflectable infos, create a dummy table (simplifies things, see PrintReflectable or Serialize)
	classDummyInfos[0] = ReflectInfo(ReflectInfo::REFLECT_TYPE_CLASS, "", 0, (PTR)(reflectable->ReflectInfosF()));
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
				if(info.infos->reflect_type == ReflectInfo::REFLECT_TYPE_VECTOR_CLASS)
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

template<> ReflectInfo* DefaultReflectInfo< bool >()               {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_BOOL,       "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< char >()               {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_CHAR,       "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< unsigned char >()      {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_UCHAR,      "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< short >()              {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_SHORT,      "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< unsigned short >()     {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_USHORT,     "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< int >()                {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_INT,        "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< unsigned int >()       {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_UINT,       "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< long >()               {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_LONG,       "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< unsigned long >()      {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_ULONG,      "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< long long >()          {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_LONGLONG,   "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< unsigned long long >() {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_ULONGLONG,  "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< float >()              {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_FLOAT,      "", 0); return &ret;}
template<> ReflectInfo* DefaultReflectInfo< double >()             {static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_DOUBLE,     "", 0); return &ret;}
