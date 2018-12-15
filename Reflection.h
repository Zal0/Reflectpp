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

class ReflectInfo;
class Reflectable;
class VectorHandlerI;
typedef ReflectInfo*(*ReflectInfosFunc)();
typedef std::auto_ptr< VectorHandlerI > VectorHandler;
typedef VectorHandler(*VectorHandlerFunc)(void*);

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

		REFLECT_TYPE_VECTOR_CLASS
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
		//if(infos->reflect_type == DefaultReflectInfo(T)->reflect_type)
		return *REFLECT_PTR(T, reflectable, infos->ptr);
	}
	ReflectField ClassPtr() const {return ReflectField(REFLECT_PTR(Reflectable, reflectable, infos->ptr), ((ReflectInfosFunc)infos->extra)());}
	VectorHandler GetVectorHandler() const;
	ReflectField Get(const char* field) const;

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

template< class R >
ReflectInfo* DefaultReflectInfo()
{
	static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_CLASS, "", 0, (PTR)R::ClassReflectInfos);
	return &ret;
}
template<> ReflectInfo* DefaultReflectInfo< bool >();
template<> ReflectInfo* DefaultReflectInfo< char >();
template<> ReflectInfo* DefaultReflectInfo< unsigned char >();
template<> ReflectInfo* DefaultReflectInfo< short >();
template<> ReflectInfo* DefaultReflectInfo< unsigned short >();
template<> ReflectInfo* DefaultReflectInfo< int >();
template<> ReflectInfo* DefaultReflectInfo< unsigned int >();
template<> ReflectInfo* DefaultReflectInfo< long >();
template<> ReflectInfo* DefaultReflectInfo< unsigned long >();
template<> ReflectInfo* DefaultReflectInfo< long long >();
template<> ReflectInfo* DefaultReflectInfo< unsigned long long >();
template<> ReflectInfo* DefaultReflectInfo< float >();
template<> ReflectInfo* DefaultReflectInfo< double >();
template<> ReflectInfo* DefaultReflectInfo< std::string >();

template< class T >
class VectorHandlerT : public VectorHandlerI
{
private:
	std::vector< T >& v;
	VectorHandlerT(void* ptr) : v(*((std::vector< T >*)ptr)){}

public:
	static VectorHandler GetVectorHandler(void* ptr) {return VectorHandler(new VectorHandlerT<T>(ptr));}
	
	virtual int GetNumElems() {return (int)v.size();}
	virtual void Push() {v.push_back(T());}
	virtual void Pop() {v.pop_back();}
	virtual void Clear() {v.clear();}

protected:
	virtual void* GetElemPtr(int idx) {return &v[idx];}
	virtual ReflectInfo* GetItemsReflectInfos() {return DefaultReflectInfo< T >();}
};

class ReflectInfoIterator {
public:
	std::vector< ReflectField > l;

public:
	ReflectInfoIterator(const ReflectField& reflectable);
	ReflectField Next();
};

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
	virtual void* This() {return this;}

	ReflectField Get(const char* field);
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

#define REFLECTABLE_CLASS(A)                                        \
class A : public ReflectableInit< A >, public virtual Reflectable { \
private:                                                            \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;}             \
                                                                    \
	static ReflectInfo* InheritanceTable() {                          \
			static ReflectInfo info[] = {                                 \
				ReflectInfo::End                                            \
			};                                                            \
			return info;                                                  \
		}

#define REFLECTABLE_CLASS_INHERITS_1(A, B)              \
class A : public ReflectableInit< A >, public B {       \
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

#define REFLECTABLE_CLASS_INHERITS_2(A, B, C)               \
class A : public ReflectableInit< A >, public B, public C { \
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

#endif
