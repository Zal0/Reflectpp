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
