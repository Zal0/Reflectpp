// Reflection.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>

#include "Reflection.h"


#define VAR_TMP(ACCESS, TYPE, NAME) ACCESS: TYPE NAME;

class Test0{
	VAR_TMP(public, int, x);
	VAR_TMP(private, int, y);
};

REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA \
		REFLECT_INT(public, Ai, 1)    \
		REFLECT_SHORT(public, As, 2)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(B)
public:
	#define REFLECTION_DATA \
		REFLECT_SHORT(public, Bi, 3)  \
		REFLECT_FLOAT(public, Bf, 4)  \
		REFLECT_CLASS(public, A, test)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS_INHERITS_2(C, A, B)
public:
	#define REFLECTION_DATA   \
		REFLECT_SHORT(public, Ci, 5)    \
		REFLECT_FLOAT(public, Cf, 6.0f) \
		REFLECT_CLASS(private, A, test)
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
	Reflectable* reflectables[3];

	reflectables[0] = new A();
	reflectables[1] = new B();
	reflectables[2] = (A*)new C();
	
	for(int i = 0; i < 3; ++i)
	{
		PrintReflectable(reflectables[i], reflectables[i]->ReflectInfos());
		printf("\n");
	}

	scanf_s("");
	return 0;
}

