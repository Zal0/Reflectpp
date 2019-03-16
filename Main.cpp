// Reflection.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>

#include "Reflection.h"
#include "ReflectionJsonTools.h"

#define ENUM_NAME testEnum
#define ENUM_ENTRIES \
	ENUM_ENTRY(enum0) \
	ENUM_ENTRY(enum1, 10) \
	ENUM_ENTRY(enum500)
#include "ReflectEnumDecl.h"

REFLECTABLE_CLASS(Test0)
#define REFLECTION_DATA \
	SERIALIZED_FIELD(public, testEnum, en, enum0) \
	SERIALIZED_FIELD(public, std::vector< testEnum >, enVector) \
	SERIALIZED_FIELD(public, int, x, 10) \
	SERIALIZED_FIELD(public, int, y, 10)
#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(A)
public:
	int exposed_int;
	#define REFLECTION_DATA       \
		SERIALIZED_FIELD(public, int, Ai, 1)  \
		SERIALIZED_FIELD(public, short, As, 2)\
		SERIALIZED_FIELD(public, std::vector< Test0 >, v_test0)\
		SERIALIZED_FIELD(public, std::vector< int >, v_int)\
		SERIALIZED_FIELD(public, Test0, array_test[3]) \
		EXPOSED_FIELD(int, exposed_int)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(B)
public:
	#define REFLECTION_DATA \
		SERIALIZED_FIELD(public, short, Bi, 3)  \
		SERIALIZED_FIELD(public, float, Bf, 4)  \
		SERIALIZED_FIELD(public, A, test)
	#include "ReflectDecl.h"

	virtual const char* ToString() {
		return "I am a B ptr";
	}
};

REFLECTABLE_CLASS_INHERITS_2(C, A, B)
public:
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, std::string, name) \
		SERIALIZED_FIELD(public, short, Ci, 5)    \
		SERIALIZED_FIELD(public, float, Cf, 6.0f) \
		SERIALIZED_FIELD(private, A, testC) \
		SERIALIZED_FIELD(public, std::vector< std::vector< testEnum > >, v_table) \
		SERIALIZED_FIELD(public, B*, bPtr)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS(D)
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, short, Fi, -5)
	#include "ReflectDecl.h"
};

REFLECTABLE_CLASS_INHERITS_1(G, A)
};

REFLECTABLE_CLASS_INHERITS_3(F, A, B, D)
};

void PrintReflectable(const ReflectField& reflectable, int depth = 0)
{
	char* tabs = new char[depth + 1];
	for(int i = 0; i < depth; ++i)
		tabs[i] = ' ';
	tabs[depth] = '\0';

	
	if(reflectable.IsArray()) //either a REFLECT_TYPE_VECTOR or an array
	{
		int num_elems = reflectable.GetNumElems();
		printf("[%d]\n", num_elems);
		for(int i = 0; i < num_elems; ++i)
		{
			printf("%s[%d] ", tabs, i);
			PrintReflectable(reflectable.GetElem(i), depth + 2);
		}
	} 
	else 
	{
		switch(reflectable.infos->info->reflect_type)
		{
			case TypeReflectInfo::ReflectType::REFLECT_TYPE_CLASS: {
				if(depth == 0)
					printf("\nclass %s", reflectable.infos->id);
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

			default:
				if(reflectable.infos->info->reflect_type == TypeReflectInfo::ReflectType::REFLECT_TYPE_POINTER || reflectable.EnumData() == 0)
				{
					printf("%s\n", reflectable.ToString().c_str());
				}
				else
				{
					printf("%s\n", EnumStrValue(reflectable));
				}
				break;
		}
	}
	delete[] tabs;
}

int main()
{
	Reflectable* reflectables[4];

	reflectables[0] = new A();
	B* b = new B();
	reflectables[1] = b;

	int tmp0 = sizeof(B);
	int tmp1 = sizeof(*b);

	C* c = new C();
	c->v_test0.push_back(Test0());
	c->v_test0.push_back(Test0());
	c->v_test0.push_back(Test0());
	c->v_test0[0].en = enum0;
	c->v_test0[1].en = enum1;
	c->v_test0[2].en = enum500;
	c->v_test0[0].enVector.push_back(enum500);
	c->v_test0[0].enVector.push_back(enum500);
	c->v_test0[0].enVector.push_back(enum1);
	c->v_test0[0].enVector.push_back(enum0);
	c->v_int.push_back(1);
	c->v_int.push_back(2);
	c->v_int.push_back(3);
	c->v_int.push_back(4);
	c->v_table.push_back(std::vector<testEnum>());
	c->v_table[0].push_back(enum500);
	c->v_table[0].push_back(enum1);
	c->v_table[0].push_back(enum0);
	c->v_table.push_back(std::vector<testEnum>());
	c->v_table[1].push_back(enum0);
	c->v_table[1].push_back(enum1);
	c->v_table[1].push_back(enum500);
	c->bPtr = b;
	reflectables[2] = c;

	reflectables[3] = new F();
	
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
	
	for(int i = 0; i < 4; ++i)
	{
		PrintReflectable(reflectables[i]);
	}

	scanf_s("");
	return 0;
}

