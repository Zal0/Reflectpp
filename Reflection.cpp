#include "Reflection.h"

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

ReflectInfoIterator::ReflectField Get(const char* field, void* reflectable, ReflectInfo* infos)
{
	ReflectInfoIterator it(reflectable, infos);
	ReflectInfoIterator::ReflectField info(0,0);
	int n;
	while((info = it.Next()).reflectable)
	{
		n = strcmpidx(info.infos->id, field);
		if(info.infos->id[n] == '\0')
		{
			if(field[n] == '\0')
			{
				if(info.infos->reflect_type == ReflectInfo::ReflectType::REFLECT_TYPE_CLASS)
					return ReflectInfoIterator::ReflectField(info.ClassPtr(), info.ReflectInfos());
				else
					return info;
			}
			else if(field[n] == '.')
			{
				if(info.infos->reflect_type == ReflectInfo::ReflectType::REFLECT_TYPE_CLASS)
					return Get(&field[n + 1], info.ClassPtr(), info.ReflectInfos());
			}
			else if(field[n] == '[')
			{
				if(info.infos->reflect_type == ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS)
				{
					char* end;
					int idx = strtol(field + n + 1, &end, 10);
					VectorHandler vector_handler = info.GetVectorHandler();
					if(*(end + 1) == '\0')
						return ReflectInfoIterator::ReflectField(vector_handler->GetElem(idx), vector_handler->GetItemsReflectInfos());
					else
						return Get(end + 2, vector_handler->GetElem(idx), vector_handler->GetItemsReflectInfos());
				}
			}
		}
	}
	return ReflectInfoIterator::ReflectField(0,0);
}

ReflectInfo ReflectInfo::End(REFLECT_TYPE_INT, "", 0); 

ReflectInfoIterator::ReflectField ReflectInfoIterator::ReflectField::Get(const char* field) 
{
	return ::Get(field, reflectable, infos);
}

ReflectInfoIterator::ReflectInfoIterator(void* reflectable, ReflectInfo* infos)
{
	l.push_back(ReflectField(reflectable, infos));
}

ReflectInfoIterator::ReflectField ReflectInfoIterator::Next() 
{
	if(l.empty())
		return ReflectField(0, 0);

	void* reflectable  = l[l.size() -1].reflectable;
	ReflectInfo* infos = (l[l.size() - 1].infos) ++;
	if(infos->id == "")
	{
		l.pop_back();
		return Next();
	}

	switch(infos->reflect_type)
	{
		case ReflectInfo::ReflectType::REFLECT_TYPE_INHERITANCE_TABLE:
			l.push_back(ReflectField(reflectable, ((ReflectInfosFunc)(infos->ptr))()));
			return Next();

		case ReflectInfo::ReflectType::REFLECT_TYPE_PARENT_CLASS: {
			Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
			l.push_back(ReflectField(classObj, ((ReflectInfosFunc)(infos->extra))()));
			return Next();
		}

		case ReflectInfo::ReflectType::REFLECT_TYPE_INT:
		case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT:
		case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT:
		case ReflectInfo::ReflectType::REFLECT_TYPE_CLASS:
		default:
			return ReflectField(reflectable, infos);
	}
}

ReflectInfoIterator::ReflectField Reflectable::Get(const char* field)
{
	return ::Get(field, This(), ReflectInfos());
}
