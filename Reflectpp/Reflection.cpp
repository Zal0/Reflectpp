#include "Reflection.h"
#include <string.h>

TypeReflectInfo TypeReflectInfo::InheritanceTable(Reflectpp::REFLECT_TYPE_INHERITANCE_TABLE, 0, 0);

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
	classDummyInfos[0] = ReflectInfo(reflectable->GetTypeReflectInfoF()(), reflectable->ReflectableClassName(), 0);
	classDummyInfos[1] = REFLECTINFO_END;

	this->reflectable = reflectable->This();
	this->infos = classDummyInfos;
}

ReflectField::ReflectField(void* reflectable, ReflectInfo* infos) : 
	reflectable(reflectable), 
	infos(infos) 
{
	classDummyInfos[0] = REFLECTINFO_END;
	classDummyInfos[1] = REFLECTINFO_END;
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

TypeReflectInfo* ReflectField::GetTypeReflectInfo() const
{
	TypeReflectInfo* ret = infos->info;
	if(ret->reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY)
	{
		ret = ((TypeReflectInfo*)infos->info->extra);
	}
	return ret;
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
		ReflectField info(0, 0);
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

void FromString(ReflectField& field, const Reflectpp::Type reflect_type, const char* str);
ReflectField& ReflectField::operator=(const char* str)
{
	Reflectpp::Type reflect_type = infos->info->reflect_type;
	if(reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY)
	{
		reflect_type = ((TypeReflectInfo*)infos->info->extra)->reflect_type;
	}

	if(reflect_type == Reflectpp::REFLECT_TYPE_POINTER)
		ReflectablePtr()->FromReflectString(str);

	FromString(*this, reflect_type, str); 
	return *this;
}

STRING ToString(const ReflectField& field, const Reflectpp::Type reflect_type);
STRING ReflectField::ToString()const
{
	Reflectpp::Type reflect_type = infos->info->reflect_type;
	if(reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY)
	{
		reflect_type = ((TypeReflectInfo*)infos->info->extra)->reflect_type;
	}

	if(reflect_type == Reflectpp::REFLECT_TYPE_POINTER)
		return STRING() + '\"' + ReflectablePtr()->ToReflectString() + '\"';

	return ::ToString(*this, reflect_type);
}

PTR ReflectField::DynamicCast(TypeReflectInfo* rf, TypeReflectInfo* t)
{
	if(rf->reflect_type != Reflectpp::REFLECT_TYPE_CLASS)
		return -1; //If it's not a class  it cannot be casted for sure

	if(rf == t)
		return 0;

	//The REFLECT_TYPE_INHERITANCE_TABLE ptr is only present when there is REFLECTION_DATA declared
	ReflectInfo* parent_class = (ReflectInfo*)(rf->extra);
	if(parent_class->info->reflect_type == Reflectpp::REFLECT_TYPE_INHERITANCE_TABLE)
	{
		parent_class = ((ReflectInfosFunc)parent_class->ptr)();
	}
	
	PTR offset;
	for(; parent_class->id[0] != 0; parent_class ++)
	{
		offset = DynamicCast(((TypeReflectInfosFunc)parent_class->info->extra)(), t);
		if(offset != -1)
		{
			return parent_class->ptr + offset;
		}
	}

	return -1; //Not found
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
	return infos->info->reflect_type == Reflectpp::REFLECT_TYPE_VECTOR || strchr(infos->id, '[');
}

int ReflectField::GetNumElems() const
{
	if(const char* idx = strchr(infos->id, '['))
	{
		return atoi(++ idx);
	}
	else if(infos->info->reflect_type == Reflectpp::REFLECT_TYPE_VECTOR) 
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
	else if(infos->info->reflect_type == Reflectpp::REFLECT_TYPE_VECTOR) 
	{
		VectorHandler vector_handler = GetVectorHandler();
		return vector_handler->GetElem(idx);
	}
	return ReflectField(0, 0);
}

ReflectInfoIterator::ReflectInfoIterator(Reflectable* reflectable)
{
	Init(ReflectField(reflectable).ClassPtr());
}

ReflectInfoIterator::ReflectInfoIterator(const ReflectField& reflectable)
{
	Init(reflectable);
}

void ReflectInfoIterator::Init(const ReflectField& reflectable)
{
	//Ignore the reflectable Reflectpp::REFLECT_TYPE_CLASS that was added during casting
	if(reflectable.infos->id[0] == '\0' && reflectable.infos->info->reflect_type == Reflectpp::REFLECT_TYPE_CLASS)
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
		case Reflectpp::REFLECT_TYPE_INHERITANCE_TABLE:
			VECTOR_PUSH(l, ReflectField(reflectable, ((ReflectInfosFunc)(infos->ptr))()));
			return Next();

		case Reflectpp::REFLECT_TYPE_PARENT_CLASS: {
			Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
			TypeReflectInfo* rf = ((TypeReflectInfosFunc)(infos->info->extra))();
			VECTOR_PUSH(l, ReflectField(classObj, (ReflectInfo*)(rf->extra)));
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

Reflectpp::Type ReflectTypeBySize(int size) {
	switch (size)
	{
		case 1:  return Reflectpp::REFLECT_TYPE_CHAR;
		case 2:  return Reflectpp::REFLECT_TYPE_SHORT;
		case 8:  return Reflectpp::REFLECT_TYPE_LONGLONG; 
		default: return Reflectpp::REFLECT_TYPE_INT;
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
		case Reflectpp::REFLECT_TYPE_CHAR:  index = (int)reflectable.Get< char >(); break;
		case Reflectpp::REFLECT_TYPE_SHORT: index = (int)reflectable.Get< short >(); break;
		default: index = reflectable.Get< int >(); break;
	}

	return reflectDatas[EnumIndex(index, reflectDatas)].str;
}