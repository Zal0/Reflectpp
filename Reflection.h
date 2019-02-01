#ifndef REFLECTION_H
#define REFLECTION_H

#include <vector>
#include <string.h>

#include <iostream>
#include <memory>

#ifdef _WIN64
	typedef long long PTR;
#else
	typedef int PTR;
#endif

#define DUMMY_ADDRESS 0x10000

//Calculates the offset of a member inside a class
#define OFFSET(FIELD) ((PTR)&((ReflectClass())->FIELD) - (PTR)(ReflectClass()))

//Calculates the offset of a class when using inheritance (note: this doesn't work if the adrees is 0x0)
#define CLASS_OFFSET(CLASS) ((PTR)((CLASS*)ReflectClass()) - (PTR)(ReflectClass()))

//Returns a pointer casted to TYPE of the data stored in INSTANCE at OFFSET
#define REFLECT_PTR(TYPE, INSTANCE, OFFSET) (TYPE*)(((char*)(INSTANCE)) + OFFSET)

#define CONCAT2(A, B)  A##B
#define CONCAT(A, B) CONCAT2(A, B)

//from https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define EXPAND( x ) x //VS workaround
#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define SERIALIZED_FIELD(...) EXPAND(GET_MACRO(__VA_ARGS__, SERIALIZED_FIELD4, SERIALIZED_FIELD3, SERIALIZED_FIELD2)(__VA_ARGS__))
#define GET_MACRO_ENUM_ENTRY(_1,_2,NAME,...) NAME
#define ENUM_ENTRY(...) EXPAND(GET_MACRO_ENUM_ENTRY(__VA_ARGS__, ENUM_ENTRY2, ENUM_ENTRY1)(__VA_ARGS__))

class ReflectInfo;
class Reflectable;
class VectorHandlerI;
typedef ReflectInfo*(*ReflectInfosFunc)();
typedef std::auto_ptr< VectorHandlerI > VectorHandler;
typedef VectorHandler(*VectorHandlerFunc)(void*);
typedef Reflectable*(*ReflectablePtrFunc)(void*);

class ReflectInfo 
{
public:
	enum ReflectType 
	{
		REFLECT_TYPE_INHERITANCE_TABLE,
		REFLECT_TYPE_PARENT_CLASS,

		REFLECT_TYPE_BOOL,
		REFLECT_TYPE_CHAR,
		REFLECT_TYPE_UCHAR,
		REFLECT_TYPE_SHORT,
		REFLECT_TYPE_USHORT,
		REFLECT_TYPE_INT,
		REFLECT_TYPE_UINT,
		REFLECT_TYPE_LONG,
		REFLECT_TYPE_ULONG,
		REFLECT_TYPE_LONGLONG,
		REFLECT_TYPE_ULONGLONG,
		REFLECT_TYPE_FLOAT,
		REFLECT_TYPE_DOUBLE,
		REFLECT_TYPE_STRING,

		REFLECT_TYPE_CLASS,

		REFLECT_TYPE_VECTOR,
		REFLECT_TYPE_POINTER
	};

	ReflectType reflect_type;
	const char* id;
	PTR ptr;
	PTR extra;

public:
	ReflectInfo() {};
	ReflectInfo(ReflectType reflect_type, const char* id, PTR ptr, PTR extra = 0) : reflect_type(reflect_type), id(id), ptr(ptr), extra(extra) {}
	static ReflectInfo End;
};

template< class R > ReflectInfo* DefaultReflectInfo();

class EnumReflectData;
class ReflectField {
private:
	ReflectInfo classDummyInfos[2]; //When casting directly from Reflectable* to Reflectfield I need to store a tmp ReflectInfo table pointing to the class

public:
	void* reflectable;
	ReflectInfo* infos;
	ReflectField(Reflectable* reflectable);
	ReflectField(void* reflectable, ReflectInfo* infos) : reflectable(reflectable), infos(infos) {}

	template< class T > T& As() const
	{
		static T default_t;
		if(infos->reflect_type == DefaultReflectInfo< T >()->reflect_type)
			return *REFLECT_PTR(T, reflectable, infos->ptr);
		else
			return default_t; //This way we avoid memory issues
	}

	ReflectField ClassPtr() const {return ReflectField(REFLECT_PTR(Reflectable, reflectable, infos->ptr), ((ReflectInfo*)infos->extra));}
	VectorHandler GetVectorHandler() const;
	ReflectField Get(const char* field) const;

	EnumReflectData* EnumData() const {return infos->extra ? ((EnumReflectData*)infos->extra) : 0;}
	Reflectable* ReflectablePtr() const {return ((ReflectablePtrFunc)infos->extra)(As< void* >());}

	ReflectField& operator=(const char* str);
	std::string ToString()const;
};

class VectorHandlerI
{
public:
	virtual int GetNumElems() = 0;
	virtual void Push() = 0;
	virtual void Pop() = 0;
	virtual void Clear() = 0;
	ReflectField GetElem(int idx) {return ReflectField(GetElemPtr(idx), GetItemsReflectInfos());}

protected:
	virtual void* GetElemPtr(int idx) = 0;
	virtual ReflectInfo* GetItemsReflectInfos() = 0;
};

template< class T >
class VectorHandlerT : public VectorHandlerI
{
private:
	std::vector< T >& v;
	VectorHandlerT(void* ptr) : v(*((std::vector< T >*)ptr)){}

public:
	static VectorHandler GetVectorHandler(void* ptr) {return VectorHandler(new VectorHandlerT< T >(ptr));}
	
	virtual int GetNumElems() {return (int)v.size();}
	virtual void Push() {v.push_back(T());}
	virtual void Pop() {v.pop_back();}
	virtual void Clear() {v.clear();}

protected:
	virtual void* GetElemPtr(int idx) {return &v[idx];}
	virtual ReflectInfo* GetItemsReflectInfos() {return DefaultReflectInfo< T >();}
};

//Given a void* this returns the pointer to Reflectable* (void* must be pointing to a T* inheriting Reflectable)
template< class T >
Reflectable* ReflectablePtr(void* ptr) {
	T* t_ptr = (T*)ptr;
	return (Reflectable*)t_ptr;
}

ReflectInfo* DefaultReflectInfo(bool*);
ReflectInfo* DefaultReflectInfo(char*);
ReflectInfo* DefaultReflectInfo(unsigned char*);
ReflectInfo* DefaultReflectInfo(short*);
ReflectInfo* DefaultReflectInfo(unsigned short*);
ReflectInfo* DefaultReflectInfo(int*);
ReflectInfo* DefaultReflectInfo(unsigned int*);
ReflectInfo* DefaultReflectInfo(long*);
ReflectInfo* DefaultReflectInfo(unsigned long*);
ReflectInfo* DefaultReflectInfo(long long*);
ReflectInfo* DefaultReflectInfo(unsigned long long*);
ReflectInfo* DefaultReflectInfo(float*);
ReflectInfo* DefaultReflectInfo(double*);
ReflectInfo* DefaultReflectInfo(std::string*);

template< class T > ReflectInfo* DefaultReflectInfo(std::vector< T >*) {
	static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_VECTOR, "", 0, (PTR)VectorHandlerT< T >::GetVectorHandler); 
	return &ret;
}

template< class R >
ReflectInfo* DefaultReflectInfo(R*)
{
	return R::DefaultReflectInfo();
}

template< class R >
ReflectInfo* DefaultReflectInfo(R**)
{
	static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_POINTER, "", 0, (PTR)(ReflectablePtrFunc)ReflectablePtr< R >); 
	return &ret;
}

template< class R >
ReflectInfo* DefaultReflectInfo()
{
	return DefaultReflectInfo((R*)0);
}

class ReflectInfoIterator {
public:
	std::vector< ReflectField > l;

public:
	ReflectInfoIterator(const ReflectField& reflectable);
	ReflectField Next();
};

ReflectInfo::ReflectType ReflectTypeBySize(int size);
class EnumReflectData {
public:
	const char* str;
	const int value;
	EnumReflectData(const char* str, const int value) : str(str), value(value) {};
};
int EnumIndex(int value, const EnumReflectData* reflectDatas);
const char* EnumStrValue(const ReflectField& reflectable);


class Reflectable
{
private:
	static ReflectInfo* ClassReflectInfos() {
		static ReflectInfo info[] = {
			ReflectInfo::End
		};
		return info;
	}

public:
	virtual ReflectInfosFunc ReflectInfosF(){return ClassReflectInfos;}
	virtual void* This() = 0;

	ReflectField Get(const char* field);

	virtual const char* ToReflectString() {return "Ptr";}
	virtual void FromReflectString(const char* str) {}
};

template< class T >
class ReflectableInit
{
public:
	ReflectableInit() {
		void (T::*init)() = &T::ReflectInit;
		((*(T*)(this)).*init)();
	}
};

//Info for inherit classes requires a pointer to the function returning their ReflectInfos and the offset of the class
//The pointer to the ReflectInfos function is not enough because the first inherited class shares its address with the class
//and the compiler cast it to the latter (so in "class A : public B, public C"  A and B share the same offset (0) and calling
//ReflectInfos to a Reflectable in that address will always return A:::ReflectInfos)
#define REFLECT_INHERIT(A) ReflectInfo(ReflectInfo::REFLECT_TYPE_PARENT_CLASS, #A, CLASS_OFFSET(A), (PTR)A::ClassReflectInfos),

#define REFLECTABLE_CLASS_DECL(A)                                   \
class A : public ReflectableInit< A >, public virtual Reflectable { 

#define REFLECTABLE_CLASS_PROPS(A)                                  \
private:                                                            \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;}             \
                                                                    \
	static ReflectInfo* InheritanceTable() {                          \
			static ReflectInfo info[] = {                                 \
				ReflectInfo::End                                            \
			};                                                            \
			return info;                                                  \
		}

#define REFLECTABLE_CLASS(A) \
REFLECTABLE_CLASS_DECL(A)    \
REFLECTABLE_CLASS_PROPS(A)


#define REFLECTABLE_CLASS_INHERITS_1_DECL(A, B)         \
class A : public ReflectableInit< A >, public B {       

#define REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)        \
private:                                                \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;} \
                                                        \
	static ReflectInfo* InheritanceTable() {              \
		static ReflectInfo info[] = {                       \
			REFLECT_INHERIT(B)                                \
			ReflectInfo::End                                  \
		};                                                  \
		return info;                                        \
	}

#define REFLECTABLE_CLASS_INHERITS_1(A, B) \
REFLECTABLE_CLASS_INHERITS_1_DECL(A, B)    \
REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)

#define REFLECTABLE_CLASS_INHERITS_2_DECL(A, B, C)          \
class A : public ReflectableInit< A >, public B, public C { 

#define REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)         \
private:                                                    \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;}     \
                                                            \
	static ReflectInfo* InheritanceTable() {                  \
		static ReflectInfo info[] = {                           \
			REFLECT_INHERIT(B)                                    \
			REFLECT_INHERIT(C)                                    \
			ReflectInfo::End                                      \
		};                                                      \
		return info;                                            \
	}


#define REFLECTABLE_CLASS_INHERITS_2(A, B, C) \
REFLECTABLE_CLASS_INHERITS_2_DECL(A, B, C)    \
REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)

#endif
