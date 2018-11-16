// Reflection.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>

#include "Reflection.h"

REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA \
		REFLECT_INT(Ai, 1)    \
		REFLECT_SHORT(As, 2)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(B)
public:
	#define REFLECTION_DATA \
		REFLECT_SHORT(Bi, 3)  \
		REFLECT_FLOAT(Bf, 4)  \
		REFLECT_CLASS(A, test)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS_INHERITS_2(C, A, B)
public:
	#define REFLECTION_DATA   \
		REFLECT_SHORT(Ci, 5)    \
		REFLECT_FLOAT(Cf, 6.0f) \
		REFLECT_CLASS(A, test)
	#include "ReflectDecl.h"
};

void PrintReflectable(void* reflectable, ReflectInfo* infos, int depth = 0) 
{
	char* tabs = new char[depth + 1];
	for(int i = 0; i < depth; ++i)
		tabs[i] = ' ';
	tabs[depth] = '\0';

	while(infos->id != "")
	{
		switch(infos->reflect_type)
		{
			case ReflectInfo::ReflectType::REFLECT_TYPE_INHERITANCE_TABLE:
				PrintReflectable(reflectable, ((ReflectInfosFunc)(infos->ptr))(), depth);
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_PARENT_CLASS: {
				Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
				PrintReflectable(classObj, ((ReflectInfosFunc)(infos->extra))(), depth);
				break;
			}

			case ReflectInfo::ReflectType::REFLECT_TYPE_INT:
				printf("%s%s: %d\n", tabs, infos->id, *REFLECT_PTR(int, reflectable, infos->ptr));
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT:
				printf("%s%s: %d\n", tabs, infos->id, *REFLECT_PTR(short, reflectable, infos->ptr));
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT:
				printf("%s%s: %f\n", tabs, infos->id, *REFLECT_PTR(float, reflectable, infos->ptr));
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_CLASS: {
				printf("%s%s:\n", tabs, infos->id);
				Reflectable* classObj = REFLECT_PTR(Reflectable, reflectable, infos->ptr);
				PrintReflectable(classObj, ((ReflectInfosFunc)(infos->extra))(), depth + 1);
				break;
			}
		}
		infos ++;
	}

	delete[] tabs;
}

/*class Test0 {
public:
	int t0i;
	Test0(){}
	void Func() {
		printf("t0");
	}
};
class Test0_2 {
public:
	int t0i;
	Test0_2();
	void Func() {
		printf("t1");
	}
};

class Test1 : public Test0, public Test0_2 {
public:
	int t1i;
	Test1() {
		printf("none");
	}
	void Func() {
		printf("me");
	}
};

Test0_2::Test0_2()
{
	Test0* t0 = (Test0*)this;
	t0->Func();
	Test0_2* t0_2 = (Test0_2*)this;
	t0_2->Func();
	Test1* t1 = (Test1*)this;
	t1->Func();
}*/

int main()
{
	//Test1 t1;

	A a;
	PrintReflectable(&a, a.ReflectInfos());
	printf("\n");

	B* b = new B();
	PrintReflectable(b, b->ReflectInfos());
	printf("\n");

	Reflectable* r = &a;
	PrintReflectable(r, r->ReflectInfos());
	printf("\n");

	C c;
	PrintReflectable(&c, c.ReflectInfos());
	printf("\n");

	scanf_s("");
	return 0;
}

