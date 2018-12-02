#include "Reflection.h"

ReflectInfo ReflectInfo::End(REFLECT_TYPE_INT, "", 0); 

ReflectField::ReflectField(Reflectable* reflectable)
{
	this->reflectable = reflectable->This();
	this->infos = reflectable->ReflectInfos();
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

ReflectField ReflectField::Get(const char* field) 
{
	ReflectInfoIterator it(*this);
	ReflectField info(0,0);
	int n;
	while((info = it.Next()).reflectable)
	{
		n = strcmpidx(info.infos->id, field);
		if(info.infos->id[n] == '\0')
		{
			if(field[n] == '\0')
			{
				if(info.infos->reflect_type == ReflectInfo::ReflectType::REFLECT_TYPE_CLASS)
					return info.ClassPtr();
				else
					return info;
			}
			else if(field[n] == '.')
			{
				if(info.infos->reflect_type == ReflectInfo::ReflectType::REFLECT_TYPE_CLASS)
					return info.ClassPtr().Get(&field[n + 1]);
			}
			else if(field[n] == '[')
			{
				if(info.infos->reflect_type == ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS)
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

ReflectInfoIterator::ReflectInfoIterator(const ReflectField& reflectable)
{
	l.push_back(reflectable);
}

ReflectField ReflectInfoIterator::Next() 
{
	if(l.empty()) //Done
		return ReflectField(0, 0);

	void* reflectable  = l[l.size() -1].reflectable;
	ReflectInfo* infos = (l[l.size() - 1].infos) ++;
	if(infos->id == "") //table end
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

		default:
			return ReflectField(reflectable, infos);
	}
}

ReflectField Reflectable::Get(const char* field)
{
	return ReflectField(this).Get(field);
}
