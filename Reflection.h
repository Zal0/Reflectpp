#ifndef REFLECTION_H
#define REFLECTION_H

#include "ReflectSTLConfig.h"

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
typedef SMART_PTR(VectorHandlerI) VectorHandler;
typedef VectorHandler(*VectorHandlerFunc)(void*);
typedef Reflectable*(*ReflectablePtrFunc)(void*);

class TypeReflectInfo
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
		REFLECT_TYPE_POINTER,
		REFLECT_TYPE_PROPERTY
	};

	ReflectType reflect_type;
	unsigned int size;
	PTR extra;

public:
	TypeReflectInfo() {};
	TypeReflectInfo(ReflectType reflect_type, unsigned int size, PTR extra) : reflect_type(reflect_type), size(size), extra(extra) {}
	static TypeReflectInfo InheritanceTable;
};

class ReflectInfo 
{
public:
	TypeReflectInfo* info;
	const char* id;
	PTR ptr;

public:
	ReflectInfo() {};
	ReflectInfo(TypeReflectInfo* info, const char* id, PTR ptr);
	static ReflectInfo End;
};

template< class R > ReflectInfo* DefaultReflectInfo();

class PropertyI
{
public:
	virtual void Get(void* t, void* ret) = 0;
	virtual void Set(void* t, void* val) = 0;
};

template< class T, class V >
class Property : public PropertyI
{
private:
	V(T::*getter)() const;
	void(T::*setter)(const V& v);

public:
	Property(V(T::*getter)() const, void(T::*setter)(const V& v)) : getter(getter), setter(setter) {}
	
	void Get(void* t, void* ret) {*((V*)ret) = (((T*)t)->*getter)();}
	void Set(void* t, void* val) {(((T*)t)->*setter)(*(V*)val);}
};

class EnumReflectData;
class ReflectField {
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
		static T default_t;
		if(infos->info->reflect_type == DefaultReflectInfo< T >()->info->reflect_type)
		{
			return *REFLECT_PTR(T, reflectable, infos->ptr);
		}
		else if(infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_PROPERTY && 
			((TypeReflectInfo*)infos->info->extra)->reflect_type ==  DefaultReflectInfo< T >()->info->reflect_type)
		{
			PropertyI* prop = (PropertyI*)infos->ptr;
			prop->Get(reflectable, &default_t);
			return default_t;
		}
		else
			return default_t; //This way we avoid memory issues
	}

	template< class T > void Set(const T& t) const
	{
		static T default_t;
		if(infos->info->reflect_type == DefaultReflectInfo< T >()->info->reflect_type)
		{
			*REFLECT_PTR(T, reflectable, infos->ptr) = t;
		}
		else if(infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_PROPERTY && 
			((TypeReflectInfo*)infos->info->extra)->reflect_type ==  DefaultReflectInfo< T >()->info->reflect_type)
		{
			PropertyI* prop = (PropertyI*)infos->ptr;
			default_t = t;
			prop->Set(reflectable, &default_t);
		}
	}

	//Content REFLECT_TYPE_CLASS
	ReflectField ClassPtr() const {return ReflectField(REFLECT_PTR(Reflectable, reflectable, infos->ptr), ((ReflectInfo*)infos->info->extra));}
	ReflectField Get(const char* field) const;

	//Content REFLECT_TYPE_VECTOR
	VectorHandler GetVectorHandler() const;

	//Content Enum
	EnumReflectData* EnumData() const {return GetTypeReflectInfo()->extra ? ((EnumReflectData*)GetTypeReflectInfo()->extra) : 0;}

	//Content REFLECT_TYPE_POINTER
	Reflectable* ReflectablePtr() const {return ((ReflectablePtrFunc)infos->info->extra)(Get< void* >());}

	bool IsArray() const;
	int GetNumElems() const;
	ReflectField GetElem(int idx) const;

	ReflectField& operator=(const char* str);
	STRING ToString()const;
};

class VectorHandlerI
{
public:
	virtual ~VectorHandlerI() {}
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
	VECTOR(T)& v;
	VectorHandlerT(void* ptr) : v(*((VECTOR(T)*)ptr)){}

public:
	static VectorHandler GetVectorHandler(void* ptr) {return VectorHandler(new VectorHandlerT< T >(ptr));}
	
	virtual int GetNumElems() {return (int)VECTOR_SIZE(v);}
	virtual void Push() {VECTOR_PUSH(v, T());}
	virtual void Pop() {VECTOR_POP(v);}
	virtual void Clear() {VECTOR_CLEAR(v);}

protected:
	virtual void* GetElemPtr(int idx) {return &VECTOR_GET(v, idx);}
	virtual ReflectInfo* GetItemsReflectInfos() {return DefaultReflectInfo((T*)0);}
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
ReflectInfo* DefaultReflectInfo(STRING*);

template< class T > ReflectInfo* DefaultReflectInfo(VECTOR(T)*) {
	static TypeReflectInfo t_info(TypeReflectInfo::REFLECT_TYPE_VECTOR, sizeof(VECTOR(T)), (PTR)VectorHandlerT< T >::GetVectorHandler);
	static ReflectInfo ret(&t_info, "", 0); 
	return &ret;
}

TypeReflectInfo::ReflectType ReflectTypeBySize(int size);

template< class R >
ReflectInfo* DefaultReflectInfo(R*)
{
	return R::DefaultReflectInfo();
}

template< class R >
ReflectInfo* DefaultReflectInfo(R**)
{
	static TypeReflectInfo t_info(TypeReflectInfo::REFLECT_TYPE_POINTER, sizeof(R*), (PTR)(ReflectablePtrFunc)ReflectablePtr< R >);
	static ReflectInfo ret(&t_info, "", 0); 
	return &ret;
}

template< class R >
ReflectInfo* DefaultReflectInfo()
{
	return DefaultReflectInfo((R*)0);
}

template< class T >
TypeReflectInfo* PropertyReflectInfo()
{
	static TypeReflectInfo ret(TypeReflectInfo::REFLECT_TYPE_PROPERTY, sizeof(void*) * 2, (PTR)DefaultReflectInfo((T*)0)->info);
	return &ret;
}

class ReflectInfoIterator {
public:
	VECTOR(ReflectField) l;

public:
	ReflectInfoIterator(const ReflectField& reflectable);
	ReflectField Next();
};

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
public:
	static ReflectInfo* DefaultReflectInfo()
	{
		static ReflectInfo info[] = {
			ReflectInfo::End
		};
		static TypeReflectInfo t_info(TypeReflectInfo::REFLECT_TYPE_CLASS, sizeof(Reflectable), (PTR)info);
		static ReflectInfo ret(&t_info, "Reflectable", 0);
		return &ret;
	}

public:
	virtual ReflectInfosFunc DefaultReflectInfoF() {return DefaultReflectInfo;}
	virtual void* This() {return this;}

	ReflectField Get(const char* field);

	virtual const char* ToReflectString() {return "Ptr";}
	virtual void FromReflectString(const char* str) {}
};

template< class T >
class ReflectableInit
{
public:
	void ReflectInit() {}

	ReflectableInit() {
		void (T::*init)() = &T::ReflectInit;
		((*(T*)(this)).*init)();
	}

	static ReflectInfo* DefaultReflectInfo() {
		static TypeReflectInfo t_info(TypeReflectInfo::REFLECT_TYPE_CLASS, sizeof(T), (PTR)T::InheritanceTable());
		static ReflectInfo ret(&t_info, T::ReflectableClassName(), 0);
		return &ret;
	}
};

//Info for inherit classes requires a pointer to the function returning their ReflectInfos and the offset of the class
//The pointer to the ReflectInfos function is not enough because the first inherited class shares its address with the class
//and the compiler cast it to the latter (so in "class A : public B, public C"  A and B share the same offset (0) and calling
//ReflectInfos to a Reflectable in that address will always return A:::ReflectInfos)
#define REFLECT_INHERIT(A) ReflectInfo(A::TypeReflectInfoParent(), #A, CLASS_OFFSET(A)),

#define REFLECTABLE_CLASS_DECL(A)                                   \
class A : private ReflectableInit< A >, public virtual Reflectable { 

#define REFLECTABLE_CLASS_COMMON_PROPS(A)                                      \
public:                                                                        \
	typedef A CLASS_NAME;                                                        \
	using ReflectableInit< A >::DefaultReflectInfo;                              \
	using ReflectableInit< A >::ReflectInit;                                     \
	virtual ReflectInfosFunc DefaultReflectInfoF() {return &DefaultReflectInfo;} \
	virtual void* This() {return this;}                                          \
	static const char* ReflectableClassName() {return #A;}                       \
	static TypeReflectInfo* TypeReflectInfoParent() {static TypeReflectInfo ret(TypeReflectInfo::REFLECT_TYPE_PARENT_CLASS, sizeof(A), (PTR)A::DefaultReflectInfo); return &ret;} \
private:                                                                       \
	friend class ReflectableInit< A >;                                           \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;}


#define REFLECTABLE_CLASS_PROPS(A)                                  \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                                 \
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
class A : private ReflectableInit< A >, public B {       

#define REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)        \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                     \
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
class A : private ReflectableInit< A >, public B, public C { 

#define REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)         \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                         \
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

#define REFLECTABLE_CLASS_INHERITS_3_DECL(A, B, C, D)                 \
class A : private ReflectableInit< A >, public B, public C, public D { 

#define REFLECTABLE_CLASS_INHERITS_3_PROPS(A, B, C, D)      \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                         \
                                                            \
	static ReflectInfo* InheritanceTable() {                  \
		static ReflectInfo info[] = {                           \
			REFLECT_INHERIT(B)                                    \
			REFLECT_INHERIT(C)                                    \
			REFLECT_INHERIT(D)                                    \
			ReflectInfo::End                                      \
		};                                                      \
		return info;                                            \
	}


#define REFLECTABLE_CLASS_INHERITS_3(A, B, C, D) \
REFLECTABLE_CLASS_INHERITS_3_DECL(A, B, C, D)    \
REFLECTABLE_CLASS_INHERITS_3_PROPS(A, B, C, D)

#endif
