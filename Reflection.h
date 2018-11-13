#ifndef REFLECTION_H
#define REFLECTION_H

#ifdef _WIN64
	typedef long long PTR;
#else
	typedef int PTR;
#endif

#define OFFSET(FIELD) ((PTR)&((ReflectClass())->FIELD) - (PTR)(ReflectClass()))
#define REFLECT_PTR(TYPE, INSTANCE, OFFSET) (TYPE*)(((char*)&(INSTANCE)) + OFFSET)

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

	const char* id;
	ReflectType reflect_type;
	PTR ptr;

public:
	ReflectInfo(ReflectType reflect_type, const char* id, PTR ptr) : reflect_type(reflect_type), id(id), ptr(ptr) {}
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

#define REFLECT_INHERIT(A) ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_PARENT_CLASS, #A, (PTR)A::ClassReflectInfos),

#define REFLECTABLE_CLASS(A)                   \
class A : public Reflectable {                 \
private:                                       \
	static A* ReflectClass() { return (A*)0x0;}  \
                                               \
	static ReflectInfo* InheritanceTable() {     \
			static ReflectInfo info[] = {            \
				ReflectInfo::End                       \
			};                                       \
			return info;                             \
		}

#define REFLECTABLE_CLASS_INHERITS_1(A, B)     \
class A : public B {                           \
private:                                       \
	static A* ReflectClass() { return (A*)0x0;}  \
                                               \
	static ReflectInfo* InheritanceTable() {     \
		static ReflectInfo info[] = {              \
			REFLECT_INHERIT(B)                       \
			ReflectInfo::End                         \
		};                                         \
		return info;                               \
	}

