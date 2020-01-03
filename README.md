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

##Installation
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

