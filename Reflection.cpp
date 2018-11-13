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

REFLECTABLE_CLASS_INHERITS_1(B, A)
public:
	#define REFLECTION_DATA \
		REFLECT_SHORT(Bi)     \
		REFLECT_FLOAT(Bf)     \
		REFLECT_CLASS(A, test)
	#include "ReflectDecl.h"
};

void PrintReflectable(const Reflectable& reflectable, ReflectInfo* infos, int depth = 0) 
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

			case ReflectInfo::ReflectType::REFLECT_TYPE_PARENT_CLASS:
				PrintReflectable(reflectable, ((ReflectInfosFunc)(infos->ptr))(), depth);
				break;

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
				PrintReflectable(*classObj, classObj->ReflectInfos(), depth + 1);
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
	PrintReflectable(a, a.ReflectInfos());

	B* b = new B();
	b->Ai = 100;
	b->As = 255;
	b->Bf = 0.12345f;
	b->Bi = 1;
	b->test.Ai = 99;
	b->test.As = 100;
	printf("\n");
	PrintReflectable(*b, b->ReflectInfos());

	Reflectable* r = &a;
	printf("\n");
	PrintReflectable(*r, r->ReflectInfos());

	scanf_s("");
	return 0;
}

