// Reflection.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>

#include "Reflection.h"
#include "JsonTools.h"

REFLECTABLE_CLASS(Test0)
#define REFLECTION_DATA \
	SERIALIZED_FIELD(public, int, x, 10) \
	SERIALIZED_FIELD(public, int, y, 10)  
#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA       \
		SERIALIZED_FIELD(public, int, Ai, 1)  \
		SERIALIZED_FIELD(public, short, As, 2)\
		SERIALIZED_FIELD_VECTOR(public, Test0, v_test0)\
		SERIALIZED_FIELD_VECTOR(public, int, v_int)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(B)
public:
	#define REFLECTION_DATA \
		SERIALIZED_FIELD(public, short, Bi, 3)  \
		SERIALIZED_FIELD(public, float, Bf, 4)  \
		SERIALIZED_FIELD_CLASS(public, A, test)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS_INHERITS_2(C, A, B)
public:
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, std::string, name, "pepe") \
		SERIALIZED_FIELD(public, short, Ci, 5)    \
		SERIALIZED_FIELD(public, float, Cf, 6.0f) \
		SERIALIZED_FIELD_CLASS(private, A, testC)
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

		default:
			printf("%s\n", reflectable.ToString().c_str());
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

	//int& n = reflectables[2]->Get("v_test0[2].y").Int();
	//n = 12345;

	//ReflectField r = reflectables[2]->Get("v_test0");
	//VectorHandler v_h = r.GetVectorHandler();
	//v_h->Push();
	//v_h->GetElem(0).Get("x").Int() = 33;
	
	//ReflectField r2 = reflectables[2]->Get("v_test0");
	//VectorHandler v_h2 = r2.GetVectorHandler();
	//r2 = ReflectField(v_h2->GetElem(0));
	//r2.Get("x").Int() = 786;

	//ReflectField r_class = reflectables[2]->Get("test");
	//r_class.Get("As").Short() = 321;

	Serialize(reflectables[2], "test.json");
	//Deserialize(reflectables[2], "test.json");
	
	for(int i = 0; i < 3; ++i)
	{
		PrintReflectable(reflectables[i]);
	}

	scanf_s("");
	return 0;
}

