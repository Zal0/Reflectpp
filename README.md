# Reflectpp

An easy to use reflection library for C++

## Basic usage
In order to add serialization to a simple class like this
```cpp
class A 
{
public:
	int a_int;
	float a_float;
};
```
- Declare your class using the macro REFLECTABLE_CLASS (remove the open bracket '{')
- Declare each field using the macro SERIALIZED_FIELD(access, type, name)
- Insert all your reflectable fields between #define REFLECTION_DATA and #include "ReflectDecl.h"

```cpp
REFLECTABLE_CLASS(A)
public:
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, int, a_int) \
		SERIALIZED_FIELD(public, float, a_float)
	#include "ReflectDecl.h"
};
```
Notice that because REFLECTION_DATA is a macro each line must end in '\' or have everything declared on one single line. You can also use EXPOSED_FIELD(type, name) to separate fields declaration from serialization definition (and keep your class intact)
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

## Acccessing fields using reflection
Given the class A initialized like this
```cpp
A a;
a.a_int = 1;
```
Use the ReflectField class to access fields using reflection
```cpp
ReflectField a_field = a.Get("a_int");
```
You can then get or set the values using the methods Get and Set
```cpp
int a_int = a_field.Get< int >(); //a_int == 1
a_field.Set(2); //a.aint == 2
```
Some extra info from the field can be extracted using the Reflectfield class like the field name
```cpp
const char* id = a_field.infos->id; //id == "a_int"
```
the field type
```cpp
Reflectpp::Type type = a_field.GetTypeReflectInfo()->reflect_type; //type == REFLECT_TYPE_INT
```
or the field size
```cpp
unsigned int size = a_field.GetTypeReflectInfo()->size; //size == sizeof(int)
```
There are also some helper functions that makes it easier to retrieve the data depending on the field type. We will see them below

## Fields iterator
Given the class A initialized like this
```cpp
A a;
a.a_int = 1;
a.a_float = 10.0f;
```
you can iterate its fields using a ReflectInfoIterator 
```cpp
ReflectInfoIterator it(&a);
ReflectField info;
while((info = it.Next()).reflectable)
{
	printf("%s: %s\n", info.infos->id, info.ToString().c_str());
}
```
    a_int: 1
    a_float: 10


