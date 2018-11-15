// Reflection.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>

#include "Reflection.h"

REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA \
		REFLECT_INT(Ai)       \
		REFLECT_SHORT(As)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(B)
public:
	#define REFLECTION_DATA \
		REFLECT_SHORT(Bi)     \
		REFLECT_FLOAT(Bf)     \
		REFLECT_CLASS(A, test)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS_INHERITS_2(C, A, B)
public:
	#define REFLECTION_DATA \
		REFLECT_SHORT(Ci)     \
		REFLECT_FLOAT(Cf)     \
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

int main()
{
	A a;
	a.Ai = 12;
	a.As = 34;
	PrintReflectable(&a, a.ReflectInfos());

	B* b = new B();
	b->Bf = 0.12345f;
	b->Bi = 1;
	b->test.Ai = 99;
	b->test.As = 100;
	printf("\n");
	PrintReflectable(b, b->ReflectInfos());

	Reflectable* r = &a;
	printf("\n");
	PrintReflectable(r, r->ReflectInfos());

	C c;
	c.A::Ai = 0;
	c.A::As = 1;
	c.Bi = 2;
	c.Bf = 3.0f;
	c.B::test.Ai = 4;
	c.B::test.As = 5;
	c.Ci = 6;
	c.Cf = 7.0f;
	c.test.Ai = 8;
	c.test.As = 9;
	printf("\n");
	PrintReflectable(&c, c.ReflectInfos());

	scanf_s("");
	return 0;
}

