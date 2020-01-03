# Reflectpp

An easy to use reflection library for C++

## Features
- User friendly, just a couple of lines on the declaration of your clases
- Declaration+Reflection on the same line
- public, private and protected fields
- Inheritance
- Composition
- Supported types:
	- Basic types:  bool, char, short, int, long, float, double
	- std::string
	- std::vector
	- arrays, multidimensional arrays
	- enums
	- properties (get/set)
	- pointers
- json import/export

## Installation
Copy the folder Reflectpp/ into your project and add it to your include path

## Basic usage
Given a simple class like this one
```cpp
class A 
{
public:
	int a_int;
	float a_float;
};
```
Add reflection data to it following the next steps:
- Include "Reflection.h"
- Declare your class using the macro REFLECTABLE_CLASS (remove the open bracket '{')
- Insert all your reflectable fields between #define REFLECTION_DATA and #include "ReflectDecl.h"
- Declare each field using the macro EXPOSED_FIELD(type, name)
```cpp
REFLECTABLE_CLASS(A)
public:
	int a_int;
	float a_float;

	#define REFLECTION_DATA   \
		EXPOSED_FIELD(int, a_int) \
		EXPOSED_FIELD(float, a_float)
	#include "ReflectDecl.h"
};
```
###### Notice that because REFLECTION_DATA is a macro each line must end in '\' or have everything declared on a single line. 

You can also use SERIALIZED_FIELD(access, type, name) to declare and add reflection info to your fields
```cpp
REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, int, a_int) \
		SERIALIZED_FIELD(public, float, a_float)
	#include "ReflectDecl.h"
};
```

## Acccessing fields using reflection
Given the class A initialized like this
```cpp
A a;
a.a_int = 1;
```

```cpp
//Getting and setting values
ReflectField a_field = a.Get("a_int");
int a_int = a_field.Get< int >(); //a_int == 1
a_field.Set(2); //a.aint == 2

//Retriving field basic info
const char* id = a_field.infos->id; //id == "a_int"
Reflectpp::Type type = a_field.GetTypeReflectInfo()->reflect_type; //type == REFLECT_TYPE_INT (supported types are in ReflectConfig.h)
unsigned int size = a_field.GetTypeReflectInfo()->size; //size == sizeof(int)
```

## Iterating Fields
Given the class A initialized like this
```cpp
A a;
a.a_int = 1;
a.a_float = 10.0f;
```
```cpp
//Iterating class fields
ReflectInfoIterator it(&a);
ReflectField info;
while((info = it.Next()).reflectable)
{
	printf("%s: %s\n", info.infos->id, info.ToString().c_str());
}
```
Output:
```
    a_int: 1
    a_float: 10
```

## Inheritance
Use the macro REFLECTABLE_CLASS_INHERITS_x where x is the number of classes you want to inherit from. The first param is the class name and the next ones the inherited classes
```cpp
REFLECTABLE_CLASS_INHERITS_1(B, A)
public:
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, int, b_int) \
		SERIALIZED_FIELD(public, float, b_float)
	#include "ReflectDecl.h"
};
```
```cpp
B b;
b.a_int = 1;
b.a_float = 10.0f;
b.b_int = 2;
b.b_float = 20.0f;

//Iterating class fields
ReflectInfoIterator it(&b);
ReflectField info;
while((info = it.Next()).reflectable)
{
	printf("%s: %s\n", info.infos->id, info.ToString().c_str());
}
```
Output:
```
a_int: 1
a_float: 10
b_int: 2
b_float: 20
```

## Composition
Just add any reflectable class to your Reflectable class like any other field
```cpp
REFLECTABLE_CLASS(C)
public:
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, int, c_int) \
		SERIALIZED_FIELD(public, float, c_float) \
		SERIALIZED_FIELD(public, A, c_a)
	#include "ReflectDecl.h"
};
```
```cpp
C c;
c.c_int = 3;
c.c_float = 30.0f;
c.c_a = a;

//Getting and setting values
ReflectField c_field = c.Get("c_a.a_int");
c_field.Set(50);
int c_a_a_int = c.c_a.a_int; //c_a_a_int == 50

PrintReflectable(&c);
```
Output:
```
class C
c_int: 3
c_float: 30
c_a:
 a_int: 50
 a_float: 10
```
Notice that beacuse c_a is a class we have to iterate all its fields recursively. The function PrintReflectable available in Main.cpp is a good example of how to iterate fields properly
