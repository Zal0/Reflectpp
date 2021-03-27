#ifndef REFLECTION_H
#define REFLECTION_H

#ifdef _WIN64
	typedef long long PTR;
#else
	typedef int PTR;
#endif

#include "ReflectConfig/ReflectConfig.h"

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

#define REFLECTINFO_END ReflectInfo(0, "", 0)

class ReflectInfo;
class TypeReflectInfo;
class Reflectable;
class VectorHandlerI;
typedef ReflectInfo*(*ReflectInfosFunc)();
typedef TypeReflectInfo*(*TypeReflectInfosFunc)();
typedef SMART_PTR(VectorHandlerI) VectorHandler;
typedef VectorHandler(*VectorHandlerFunc)(void*);
typedef Reflectable*(*ReflectablePtrFunc)(void*);

class ReflectInfo 
{
public:
	TypeReflectInfo* info;
	const char* id;
	PTR offset;

public:
	ReflectInfo() {};
	ReflectInfo(TypeReflectInfo* info, const char* id, PTR offset);
};

template< class R > TypeReflectInfo* GetTypeReflectInfo();

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
		static const T default_t = T();
		if(infos->info->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//The type matches the requested type
			return *REFLECT_PTR(T, reflectable, infos->offset);
		}
		else if(infos->info->reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY && 
			((TypeReflectInfo*)infos->info->extra)->reflect_type ==  ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//This is a property and the type matches
			PropertyI* prop = (PropertyI*)infos->offset;
			T ret;
			prop->Get(reflectable, &ret);
			return ret;
		}
		else
			//The types doesn't match
			return default_t; //This way we avoid memory issues
	}

	template< class T > void Set(const T& t) const
	{
		static T default_t;
		if(infos->info->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//The type matches the requested type
			*REFLECT_PTR(T, reflectable, infos->offset) = t;
		}
		else if(infos->info->reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY && 
			((TypeReflectInfo*)infos->info->extra)->reflect_type == ::GetTypeReflectInfo< T >()->reflect_type)
		{
			//This is a property and the type matches
			PropertyI* prop = (PropertyI*)infos->offset;
			default_t = t;
			prop->Set(reflectable, &default_t);
		}
	}

	//Content REFLECT_TYPE_CLASS
	ReflectField ClassPtr() const {return ReflectField(REFLECT_PTR(Reflectable, reflectable, infos->offset), ((ReflectInfo*)infos->info->extra));}
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

	template <class T> T* DynamicCast()
	{
		return (T*)DynamicCast(T::GetTypeReflectInfo());
	}

	void* DynamicCast(TypeReflectInfo* t);

	static bool DynamicCast(TypeReflectInfo* rf, TypeReflectInfo* t, int* offset = 0);
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
	virtual ReflectInfo* GetItemsReflectInfos() {static ReflectInfo r_info(GetTypeReflectInfo((T*)0), "", 0); return &r_info;}
};

//Given a void* this returns the pointer to Reflectable* (void* must be pointing to a T* inheriting Reflectable)
template< class T >
Reflectable* ReflectablePtr(void* ptr) {
	T* t_ptr = (T*)ptr;
	return (Reflectable*)t_ptr;
}

template< class T > 
TypeReflectInfo* GetTypeReflectInfo(VECTOR(T)*) {
	static TypeReflectInfo t_info(Reflectpp::REFLECT_TYPE_VECTOR, sizeof(VECTOR(T)), (PTR)VectorHandlerT< T >::GetVectorHandler);
	return &t_info;
}

Reflectpp::Type ReflectTypeBySize(int size);

template< class R >
TypeReflectInfo* GetTypeReflectInfo(R*)
{
	return R::GetTypeReflectInfo();
}

template< class R >
TypeReflectInfo* GetTypeReflectInfo(R**)
{
	static TypeReflectInfo t_info(Reflectpp::REFLECT_TYPE_POINTER, sizeof(R*), (PTR)(ReflectablePtrFunc)ReflectablePtr< R >);
	return &t_info;
}

template< class R >
TypeReflectInfo* GetTypeReflectInfo()
{
	return GetTypeReflectInfo((R*)0);
}

template< class T >
TypeReflectInfo* PropertyReflectInfo()
{
	static TypeReflectInfo ret(Reflectpp::REFLECT_TYPE_PROPERTY, sizeof(void*) * 2, (PTR)GetTypeReflectInfo((T*)0));
	return &ret;
}

class ReflectInfoIterator {
public:
	VECTOR(ReflectField) l;

private:
	void Init(const ReflectField& reflectable);

public:
	ReflectInfoIterator(Reflectable* reflectable);
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
	static TypeReflectInfo* GetTypeReflectInfo()
	{
		static ReflectInfo info[] = {
			REFLECTINFO_END
		};
		static TypeReflectInfo t_info(Reflectpp::REFLECT_TYPE_CLASS, sizeof(Reflectable), (PTR)info);
		return &t_info;
	}

public:
	virtual TypeReflectInfosFunc GetTypeReflectInfoF() {return GetTypeReflectInfo;}
	virtual void* This() {return this;}
	virtual const char* ReflectableClassName() {return 0;}

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

	static TypeReflectInfo* GetTypeReflectInfo() {
		static TypeReflectInfo t_info(Reflectpp::REFLECT_TYPE_CLASS, sizeof(T), (PTR)T::InheritanceTable());
		return &t_info;
	}
};

//Info for inherit classes requires a pointer to the function returning their ReflectInfos and the offset of the class
//The pointer to the ReflectInfos function is not enough because the first inherited class shares its address with the class
//and the compiler cast it to the latter (so in "class A : public B, public C"  A and B share the same offset (0) and calling
//ReflectInfos to a Reflectable in that address will always return A:::ReflectInfos)
#define REFLECT_INHERIT(A) ReflectInfo(A::TypeReflectInfoParent(), #A, CLASS_OFFSET(A)),

#define REFLECTABLE_CLASS_DECL(A)                                   \
class A : private ReflectableInit< A >, public virtual Reflectable

#define REFLECTABLE_CLASS_COMMON_PROPS(A)                                            \
public:                                                                              \
	typedef A CLASS_NAME;                                                              \
	using ReflectableInit< A >::GetTypeReflectInfo;                                    \
	using ReflectableInit< A >::ReflectInit;                                           \
	virtual TypeReflectInfosFunc GetTypeReflectInfoF() {return &GetTypeReflectInfo;}   \
	virtual void* This() {return this;}                                                \
	virtual const char* ReflectableClassName() {static const char* nm = #A; return nm;}\
	static TypeReflectInfo* TypeReflectInfoParent() {static TypeReflectInfo ret(Reflectpp::REFLECT_TYPE_PARENT_CLASS, sizeof(A), (PTR)A::GetTypeReflectInfo); return &ret;} \
private:                                                                             \
	friend class ReflectableInit< A >;                                                 \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;}


#define REFLECTABLE_CLASS_PROPS(A)                                  \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                                 \
                                                                    \
	static ReflectInfo* InheritanceTable() {                          \
			static ReflectInfo info[] = {                                 \
				REFLECTINFO_END                                             \
			};                                                            \
			return info;                                                  \
		}

#define REFLECTABLE_CLASS(A) \
REFLECTABLE_CLASS_DECL(A) {  \
REFLECTABLE_CLASS_PROPS(A)


#define REFLECTABLE_CLASS_INHERITS_1_DECL(A, B)         \
class A : private ReflectableInit< A >, public B      

#define REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)        \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                     \
                                                        \
	static ReflectInfo* InheritanceTable() {              \
		static ReflectInfo info[] = {                       \
			REFLECT_INHERIT(B)                                \
			REFLECTINFO_END                                  \
		};                                                  \
		return info;                                        \
	}

#define REFLECTABLE_CLASS_INHERITS_1(A, B) \
REFLECTABLE_CLASS_INHERITS_1_DECL(A, B) {  \
REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)

#define REFLECTABLE_CLASS_INHERITS_2_DECL(A, B, C)          \
class A : private ReflectableInit< A >, public B, public C 

#define REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)         \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                         \
                                                            \
	static ReflectInfo* InheritanceTable() {                  \
		static ReflectInfo info[] = {                           \
			REFLECT_INHERIT(B)                                    \
			REFLECT_INHERIT(C)                                    \
			REFLECTINFO_END                                      \
		};                                                      \
		return info;                                            \
	}


#define REFLECTABLE_CLASS_INHERITS_2(A, B, C) \
REFLECTABLE_CLASS_INHERITS_2_DECL(A, B, C) {  \
REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)

#define REFLECTABLE_CLASS_INHERITS_3_DECL(A, B, C, D)                 \
class A : private ReflectableInit< A >, public B, public C, public D

#define REFLECTABLE_CLASS_INHERITS_3_PROPS(A, B, C, D)      \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                         \
                                                            \
	static ReflectInfo* InheritanceTable() {                  \
		static ReflectInfo info[] = {                           \
			REFLECT_INHERIT(B)                                    \
			REFLECT_INHERIT(C)                                    \
			REFLECT_INHERIT(D)                                    \
			REFLECTINFO_END                                      \
		};                                                      \
		return info;                                            \
	}


#define REFLECTABLE_CLASS_INHERITS_3(A, B, C, D) \
REFLECTABLE_CLASS_INHERITS_3_DECL(A, B, C, D) {  \
REFLECTABLE_CLASS_INHERITS_3_PROPS(A, B, C, D)

#endif
