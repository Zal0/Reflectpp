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
	#define REFLECTION_DATA       \
		REFLECT_INT(public, Ai, 1)  \
		REFLECT_SHORT(public, As, 2)\
		REFLECT_VECTOR_CLASS(public, Test0, v_test0)
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

	ReflectInfoIterator it(reflectable, infos);
	ReflectInfoIterator::Reflectable_Info info(0,0);
	while((info = it.Next()).reflectable)
	{
		switch (info.infos->reflect_type)
		{
			case ReflectInfo::ReflectType::REFLECT_TYPE_INT:
				printf("%s%s: %d\n", tabs, info.infos->id, info.Int());
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT:
				printf("%s%s: %d\n", tabs, info.infos->id, info.Short());
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT:
				printf("%s%s: %f\n", tabs, info.infos->id, info.Float());
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_CLASS: {
				printf("%s%s:\n", tabs, info.infos->id);
				PrintReflectable(info.ClassPtr(), info.ReflectInfos(), depth + 1);
				break;
			}

			case ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS: {
				VectorHandler vector_handler = info.GetVectorHandler();
				printf("%s%s[] size:%d\n", tabs, info.infos->id, vector_handler->GetNumElems());
				for(int i = 0; i < vector_handler->GetNumElems(); ++i)
				{
					printf("%s [%d]\n", tabs, i);
					PrintReflectable(vector_handler->GetElem(i), vector_handler->GetItemsReflectInfos(), depth + 2);
				}
				break;
			}
		}
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

