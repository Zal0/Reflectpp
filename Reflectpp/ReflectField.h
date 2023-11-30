#ifndef REFLECTFIELD_H
#define REFLECTFIELD_H

#include "ReflectInfo.h"
#include "ReflectConfig/ReflectConfigTypes.h"
#include "ReflectProperty.h"
class Reflectable;
class VectorHandlerI;

class ReflectInfo;
class TypeReflectInfo;
class Reflectable;
class EnumReflectData;
typedef ReflectInfo* (*ReflectInfosFunc)();
typedef TypeReflectInfo* (*TypeReflectInfosFunc)();
typedef SMART_PTR(VectorHandlerI) VectorHandler;
typedef VectorHandler(*VectorHandlerFunc)(void*);
typedef Reflectable* (*ReflectablePtrFunc)(void*);

template< class R > TypeReflectInfo* GetTypeReflectInfo();

class ReflectField
{
private:
	ReflectInfo classDummyInfos[2]; //When casting directly from Reflectable* to Reflectfield I need to store a tmp ReflectInfo table pointing to the class
	int GetArrayElemSize() const;

public:
	void* reflectable;
	ReflectInfo* infos;
	ReflectField(Reflectable* reflectable);
	ReflectField(void* reflectable = 0, ReflectInfo* infos = 0);
	ReflectField(const ReflectField& r);

	TypeReflectInfo* GetTypeReflectInfo() const;

	template< class T > T Get() const
	{
		if(infos->info->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//The type matches the requested type
			return ReflectFieldGet(*this, (T*)0);
		}
		else if(infos->info->reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY &&
			((TypeReflectInfo*)infos->info->extra)->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//This is a property and the type matches
			PropertyI* prop = (PropertyI*)infos->offset;
			
			T ret;
			prop->Get(reflectable, &ret);

			ReflectInfo ri(((TypeReflectInfo*)infos->info->extra), "", 0);
			ReflectField rf(&ret, &ri);
			return rf.Get< T >();
		}
		else
			//The types don't match
			return T(); //This way we avoid memory issues
	}

	template< class T > void Set(const T& t)
	{
		if(infos->info->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//The type matches the requested type
			ReflectFieldSet(*this, t, (T*)0);
		}
		else if(infos->info->reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY &&
			((TypeReflectInfo*)infos->info->extra)->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//This is a property and the type matches
			PropertyI* prop = (PropertyI*)infos->offset;

			//Set on a temp var and then assing with the property setter
			T tmp;
			ReflectInfo ri = ReflectInfo(((TypeReflectInfo*)infos->info->extra), "", 0);
			ReflectField rf(&tmp, &ri);
			rf.Set< T >(t);

			prop->Set(reflectable, (T*)&tmp);
		}
	}

	//Content REFLECT_TYPE_CLASS
	ReflectField ClassPtr() const { return ReflectField(REFLECT_PTR(Reflectable, reflectable, infos->offset), ((ReflectInfo*)infos->info->extra)); }
	ReflectField Get(const char* field) const;

	//Content REFLECT_TYPE_VECTOR
	VectorHandler GetVectorHandler() const;

	//Content Enum
	EnumReflectData* EnumData() const { return GetTypeReflectInfo()->extra ? ((EnumReflectData*)GetTypeReflectInfo()->extra) : 0; }

	//Content REFLECT_TYPE_POINTER
	Reflectable* ReflectablePtr() const { return ((ReflectablePtrFunc)infos->info->extra)(Get< void* >()); }

	bool IsArray() const;
	int GetNumElems() const;
	ReflectField GetElem(int idx) const;

	ReflectField& operator=(const char* str);
	STRING ToString()const;

	template <class T> T* DynamicCast()
	{
		return (T*)DynamicCast(T::GetTypeReflectInfo());
	}

	void* DynamicCast(TypeReflectInfo* t);

	static bool DynamicCast(TypeReflectInfo* rf, TypeReflectInfo* t, int* offset = 0);
};

//These two can be overriden on ReflectConfig.h with custom types
template< class T >
T ReflectFieldGet(const ReflectField& rf, T*)
{
	return *REFLECT_PTR(T, rf.reflectable, rf.infos->offset);
}

template< class T >
void ReflectFieldSet(const ReflectField& rf, const T& t, T*)
{
	*REFLECT_PTR(T, rf.reflectable, rf.infos->offset) = t;
}

#endif