// Reflection.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>

#include "Reflection.h"
#include "JsonTools.h"

REFLECTABLE_CLASS(Test0)
#define REFLECTION_DATA       \
	REFLECT_INT(public, x, 10)  \
	REFLECT_INT(public, y, 10)  
#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA       \
		REFLECT_INT(public, Ai, 1)  \
		REFLECT_SHORT(public, As, 2)\
		REFLECT_VECTOR_CLASS(public, Test0, v_test0)\
		REFLECT_VECTOR_CLASS(public, int, v_int)
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
		REFLECT_CLASS(private, A, testC)
	#include "ReflectDecl.h"
};

void PrintReflectable(const ReflectField& reflectable, int depth = 0)
{
	char* tabs = new char[depth + 1];
	for(int i = 0; i < depth; ++i)
		tabs[i] = ' ';
	tabs[depth] = '\0';

	switch(reflectable.infos->reflect_type)
	{
		case ReflectInfo::ReflectType::REFLECT_TYPE_CLASS: {
			printf("\n");
			ReflectInfoIterator it(reflectable.ClassPtr());
			ReflectField info(0,0);
			while((info = it.Next()).reflectable)
			{
				printf("%s%s: ", tabs, info.infos->id);
				PrintReflectable(info, depth + 1);
			}
			break;
		}

		case ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS: {
			VectorHandler vector_handler = reflectable.GetVectorHandler();
			printf("[%d]\n", vector_handler->GetNumElems());
			for(int i = 0; i < vector_handler->GetNumElems(); ++i)
			{
				printf("%s[%d] ", tabs, i);
				PrintReflectable(vector_handler->GetElem(i), depth + 2);
			}
			break;
		}

		case ReflectInfo::ReflectType::REFLECT_TYPE_INT:
			printf("%d\n", reflectable.Int());
			break;

		case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT:
			printf("%d\n", reflectable.Short());
			break;

		case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT:
			printf("%f\n", reflectable.Float());
			break;
	}
	delete[] tabs;
}

int main()
{
	Reflectable* reflectables[3];

	reflectables[0] = new A();
	reflectables[1] = new B();

	C* c = new C();
	c->v_test0.push_back(Test0());
	c->v_test0.push_back(Test0());
	c->v_test0.push_back(Test0());
	c->v_int.push_back(1);
	c->v_int.push_back(2);
	c->v_int.push_back(3);
	c->v_int.push_back(4);
	reflectables[2] = c;
	
	Serialize(reflectables[2], "test.json");
	Deserialize(reflectables[2], "test.json");

	for(int i = 0; i < 3; ++i)
	{
		PrintReflectable(reflectables[i]);
	}

	//int& n = reflectables[0]->Get("v_test0[2].y").Int();
	//n = 12345;

	//ReflectField r = reflectables[2]->Get("v_test0");
	//VectorHandler v_h = r.GetVectorHandler();
	//v_h->Push();
	//Reflectable::Get("x", v_h->GetElem(0), v_h->GetItemsReflectInfos()).Int() = 1010;
	//r = ReflectField(v_h->GetElem(0));
	//r.Get("x").Int() = 786;

	//TOOD: GetElem return ReflectField
	//r.Get("x").Int() = 1010;
	//Reflectable::Get("x", r.reflectable, r.infos).Int() = 1010;

	//ReflectField r_class = reflectables[2]->Get("test");
	//r_class.Get("As").Short() = 321;

	//Serialize(reflectables[2], "test.json");

	//Deserialize(reflectables[2], "test.json");

	scanf_s("");
	return 0;
}

