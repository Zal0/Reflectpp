# Reflectpp

An easy to use reflection library for C++

## Features
- User friendly, just a couple of lines on the declaration of your clases
- Portable, works on any c++ compiler
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

## Usage
Instead of declaring your classes like
```cpp
enum Level
{
	LOW = 1,
	MEDIUM = 5,
	HIGH = 10,
};

class N {
public:
	int n_int;
	float n_float;
	std::string n_string;
	Vector3 n_vector3;
	std::vector< int > n_float_vector_array[2];
	Level level;

	N() : n_int(0), n_float(1.0f), n_string("str"), level(LOW) {
		n_float_vector_array[0].push_back(1);
		n_float_vector_array[0].push_back(2);

		n_float_vector_array[1].push_back(10);
		n_float_vector_array[1].push_back(20);
		n_float_vector_array[1].push_back(30);
	}
};
```
Use the corresponding Reflectpp macros
```cpp
#define ENUM_NAME Level
#define ENUM_ENTRIES \
	ENUM_ENTRY(LOW, 1) \
	ENUM_ENTRY(MEDIUM, 5) \
	ENUM_ENTRY(HIGH, 10)
#include "ReflectEnumDecl.h"

REFLECTABLE_CLASS(N)
	#define REFLECTION_DATA   \
		SERIALIZED_FIELD(public, int, n_int)\
		SERIALIZED_FIELD(public, float, n_float)\
		SERIALIZED_FIELD(public, std::string, n_string)\
		SERIALIZED_FIELD(public, Vector3, n_vector3)\
		SERIALIZED_FIELD(public, std::vector< int >, n_float_vector_array[2])\
		SERIALIZED_FIELD(public, Level, level)
	#include "ReflectDecl.h"

	N() : n_int(0), n_float(1.0f), n_string("str"), level(LOW) {
		n_float_vector_array[0].push_back(1);
		n_float_vector_array[0].push_back(2);

		n_float_vector_array[1].push_back(10);
		n_float_vector_array[1].push_back(20);
		n_float_vector_array[1].push_back(30);
	}
};
```
then you will be able to
```cpp
N n;

//Iterate N contents
ReflectInfoIterator it(&n);
ReflectField info;
while((info = it.Next()).reflectable)
{
	printf("%s: %s\n", info.infos->id, info.ToString().c_str());
}


//Access fields using reflection
n.Get("n_int").Set< int >(4); //n.n_int == 4
n.Get("n_float_vector_array[0][0]").Set< int >(999); //n_float_vector_array[0][0] == 99
n.Get("n_float_vector_array[0]").GetVectorHandler()->Push(); //added new element into n_float_vector_array[0] 
PrintReflectable(&n);

//Serialize into json
Serialize(&n, "n.json");

//Deserialize from json
Deserialize(&n, "n_mod.json");
PrintReflectable(&n);
```


Please refer to the [wiki](https://github.com/Zal0/Reflectpp/wiki "wiki") for detailed instructions
