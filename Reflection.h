#ifndef REFLECTION_H
#define REFLECTION_H

#ifdef _WIN64
	typedef long long PTR;
#else
	typedef int PTR;
#endif

//Calculates the offset of a member inside a class
#define OFFSET(FIELD) ((PTR)&((ReflectClass())->FIELD) - (PTR)(ReflectClass()))

//Calculates the offset of a class when using inheritance
#define CLASS_OFFSET(CLASS) ((PTR)((CLASS*)ReflectClass()) - (PTR)(ReflectClass()))

//Returns a pointer casted to TYPE of the data stored in INSTANCE at OFFSET
#define REFLECT_PTR(TYPE, INSTANCE, OFFSET) (TYPE*)(((char*)(INSTANCE)) + OFFSET)

class ReflectInfo 
{
public:
	enum ReflectType 
	{
		REFLECT_TYPE_INHERITANCE_TABLE,
		REFLECT_TYPE_PARENT_CLASS,
		REFLECT_TYPE_INT,
		REFLECT_TYPE_SHORT,
		REFLECT_TYPE_FLOAT,
		REFLECT_TYPE_CLASS
	};

	ReflectType reflect_type;
	const char* id;
	PTR ptr;
	PTR extra;

public:
	ReflectInfo(ReflectType reflect_type, const char* id, PTR ptr, PTR extra = 0) : reflect_type(reflect_type), id(id), ptr(ptr), extra(extra) {}
	static ReflectInfo End; 
};
ReflectInfo ReflectInfo::End(REFLECT_TYPE_INT, "", 0);
typedef ReflectInfo*(*ReflectInfosFunc)();

class Reflectable
{
public:
	static ReflectInfo* ClassReflectInfos() {
		static ReflectInfo info[] = {
			ReflectInfo::End
		};
		return info;
	}

	virtual ReflectInfo* ReflectInfos() {return ClassReflectInfos();}
};

#endif

//Info for inherit classes requires a pointer to the function returning their ReflectInfos and the offset of the class
//The pointer to the ReflectInfos function is not enough because the first inherited class shares its address with the class
//and the compiler cast it to the latter (so in "class A : public B, public C"  A and B share the same offset (0) and calling
//ReflectInfos to a Reflectable in that address will always return A:::ReflectInfos)
#define REFLECT_INHERIT(A) ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_PARENT_CLASS, #A, CLASS_OFFSET(A), (PTR)A::ClassReflectInfos),

#define REFLECTABLE_CLASS(A)                      \
class A : public Reflectable {                    \
private:                                          \
	static A* ReflectClass() { return (A*)0x10000;} \
                                                  \
	static ReflectInfo* InheritanceTable() {        \
			static ReflectInfo info[] = {               \
				ReflectInfo::End                          \
			};                                          \
			return info;                                \
		}

#define REFLECTABLE_CLASS_INHERITS_1(A, B)         \
class A : public B {                               \
private:                                           \
	static A* ReflectClass() { return (A*)0x10000;}  \
                                                   \
	static ReflectInfo* InheritanceTable() {         \
		static ReflectInfo info[] = {                  \
			REFLECT_INHERIT(B)                           \
			ReflectInfo::End                             \
		};                                             \
		return info;                                   \
	}

#define REFLECTABLE_CLASS_INHERITS_2(A, B, C)      \
class A : public B, public C {                     \
private:                                           \
	static A* ReflectClass() { return (A*)0x10000;}  \
                                                   \
	static ReflectInfo* InheritanceTable() {         \
		static ReflectInfo info[] = {                  \
			REFLECT_INHERIT(B)                           \
			REFLECT_INHERIT(C)                           \
			ReflectInfo::End                             \
		};                                             \
		return info;                                   \
	}
