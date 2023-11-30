#ifndef REFLECTION_H
#define REFLECTION_H

#include "ReflectConfig/ReflectConfig.h"
#include "ReflectInfo.h"
#include "ReflectField.h"
#include "VectorHandler.h"
#include "ReflectInfoIterator.h"
#include "EnumReflectData.h"

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

//This can be overriden in ReflectConfig.h to customize some classes (useful for Dlls)
template< class T > TypeReflectInfo* GetTypeReflectInfoOverride(T*) { return 0; }

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
		TypeReflectInfo* trf_override = GetTypeReflectInfoOverride((T*)0);
		return trf_override ? trf_override : &t_info;
	}
};

//Info for inherit classes requires a pointer to the function returning their ReflectInfos and the offset of the class
//The pointer to the ReflectInfos function is not enough because the first inherited class shares its address with the class
//and the compiler cast it to the latter (so in "class A : public B, public C"  A and B share the same offset (0) and calling
//ReflectInfos to a Reflectable in that address will always return A:::ReflectInfos)
#define REFLECT_INHERIT(A) ReflectInfo(A::TypeReflectInfoParent(), #A, CLASS_OFFSET(A)),

#define REFLECTABLE_CLASS_DECL(A)                                    \
class A : protected ReflectableInit< A >, public virtual Reflectable

#define REFLECTABLE_CLASS_COMMON_PROPS(A)                                                                        \
public:                                                                                                          \
	typedef A CLASS_NAME;                                                                                        \
	using ReflectableInit< A >::GetTypeReflectInfo;                                                              \
	using ReflectableInit< A >::ReflectInit;                                                                     \
	virtual TypeReflectInfosFunc GetTypeReflectInfoF() {return &GetTypeReflectInfo;}                             \
	virtual void* This() {return this;}                                                                          \
	virtual const char* ReflectableClassName() {static const char* nm = #A; return nm;}                          \
	static TypeReflectInfo* TypeReflectInfoParent()                                                              \
	{                                                                                                            \
		static TypeReflectInfo ret(Reflectpp::REFLECT_TYPE_PARENT_CLASS, sizeof(A), (PTR)A::GetTypeReflectInfo); \
		return &ret;                                                                                             \
	}                                                                                                            \
private:                                                                                                         \
	friend class ReflectableInit< A >;                                                                           \
	static A* ReflectClass() { return (A*)DUMMY_ADDRESS;}


#define REFLECTABLE_CLASS_PROPS(A)                                    \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                                 \
                                                                      \
	static ReflectInfo* InheritanceTable() {                          \
			static ReflectInfo info[] = {                             \
				REFLECTINFO_END                                       \
			};                                                        \
			return info;                                              \
		}

#define REFLECTABLE_CLASS(A) \
REFLECTABLE_CLASS_DECL(A) {  \
REFLECTABLE_CLASS_PROPS(A)


#define REFLECTABLE_CLASS_INHERITS_1_DECL(A, B)         \
class A : private ReflectableInit< A >, public B      

#define REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)        \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                   \
                                                        \
	static ReflectInfo* InheritanceTable() {            \
		static ReflectInfo info[] = {                   \
			REFLECT_INHERIT(B)                          \
			REFLECTINFO_END                             \
		};                                              \
		return info;                                    \
	}

#define REFLECTABLE_CLASS_INHERITS_1(A, B) \
REFLECTABLE_CLASS_INHERITS_1_DECL(A, B) {  \
REFLECTABLE_CLASS_INHERITS_1_PROPS(A, B)

#define REFLECTABLE_CLASS_INHERITS_2_DECL(A, B, C)          \
class A : private ReflectableInit< A >, public B, public C 

#define REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)         \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                       \
                                                            \
	static ReflectInfo* InheritanceTable() {                \
		static ReflectInfo info[] = {                       \
			REFLECT_INHERIT(B)                              \
			REFLECT_INHERIT(C)                              \
			REFLECTINFO_END                                 \
		};                                                  \
		return info;                                        \
	}


#define REFLECTABLE_CLASS_INHERITS_2(A, B, C) \
REFLECTABLE_CLASS_INHERITS_2_DECL(A, B, C) {  \
REFLECTABLE_CLASS_INHERITS_2_PROPS(A, B, C)

#define REFLECTABLE_CLASS_INHERITS_3_DECL(A, B, C, D)                 \
class A : private ReflectableInit< A >, public B, public C, public D

#define REFLECTABLE_CLASS_INHERITS_3_PROPS(A, B, C, D)      \
	REFLECTABLE_CLASS_COMMON_PROPS(A)                       \
                                                            \
	static ReflectInfo* InheritanceTable() {                \
		static ReflectInfo info[] = {                       \
			REFLECT_INHERIT(B)                              \
			REFLECT_INHERIT(C)                              \
			REFLECT_INHERIT(D)                              \
			REFLECTINFO_END                                 \
		};                                                  \
		return info;                                        \
	}


#define REFLECTABLE_CLASS_INHERITS_3(A, B, C, D) \
REFLECTABLE_CLASS_INHERITS_3_DECL(A, B, C, D) {  \
REFLECTABLE_CLASS_INHERITS_3_PROPS(A, B, C, D)

#endif
