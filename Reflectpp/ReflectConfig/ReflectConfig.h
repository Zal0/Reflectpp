#ifndef REFLECT_TYPES
#define REFLECT_TYPES

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>

#define VECTOR(TYPE) std::vector< TYPE >
#define VECTOR_SIZE(V) V.size()
#define VECTOR_PUSH(V,i) V.push_back(i)
#define VECTOR_POP(V) V.pop_back()
#define VECTOR_CLEAR(V) V.clear()
#define VECTOR_GET(V, n) V[n]
#define VECTOR_IS_EMPTY(V) V.empty()

#define STRING std::string
#define STRING_TO_CHAR_PTR(STR) STR.c_str()
#define STRING_CLEAR(STR) STR.clear()
#define STRING_APPEND(STR, C) STR += c

#define SMART_PTR(A) std::auto_ptr< A >

#define FILE_OUT std::ostream&
#define FILE_OUT_LOAD(FILE,PATH) std::ofstream FILE(PATH)
#define FILE_WRITE_STRING(FILE,STR) FILE << STR
#define FILE_WRITE_CHAR(FILE,C) FILE << C
#define FILE_OUT_CLOSE(FILE) FILE.close()

#define FILE_IN std::istream&
#define FILE_IN_LOAD(FILE, PATH) std::ifstream FILE(PATH, std::ios::binary)
#define FILE_READ_CHAR(FILE) FILE.get()
#define FILE_IN_CLOSE(FILE) FILE.close()

namespace Reflectpp
{
	enum Type
	{
		//Reflectpp types, do not modify
		REFLECT_TYPE_INHERITANCE_TABLE,
		REFLECT_TYPE_PARENT_CLASS,
		
		REFLECT_TYPE_CLASS,
		REFLECT_TYPE_VECTOR,
		REFLECT_TYPE_POINTER,
		REFLECT_TYPE_PROPERTY,

		//Custom types
		REFLECT_TYPE_BOOL,
		REFLECT_TYPE_CHAR,
		REFLECT_TYPE_UCHAR,
		REFLECT_TYPE_SHORT,
		REFLECT_TYPE_USHORT,
		REFLECT_TYPE_INT,
		REFLECT_TYPE_UINT,
		REFLECT_TYPE_LONG,
		REFLECT_TYPE_ULONG,
		REFLECT_TYPE_LONGLONG,
		REFLECT_TYPE_ULONGLONG,
		REFLECT_TYPE_FLOAT,
		REFLECT_TYPE_DOUBLE,
		REFLECT_TYPE_STRING
	};
};

class TypeReflectInfo
{
public:
	Reflectpp::Type reflect_type;
	unsigned int size;
	PTR extra;

public:
	TypeReflectInfo() {};
	TypeReflectInfo(Reflectpp::Type reflect_type, unsigned int size, PTR extra) : reflect_type(reflect_type), size(size), extra(extra) {}
	static TypeReflectInfo InheritanceTable;
};

TypeReflectInfo* GetTypeReflectInfo(bool*);
TypeReflectInfo* GetTypeReflectInfo(char*);
TypeReflectInfo* GetTypeReflectInfo(unsigned char*);
TypeReflectInfo* GetTypeReflectInfo(short*);
TypeReflectInfo* GetTypeReflectInfo(unsigned short*);
TypeReflectInfo* GetTypeReflectInfo(int*);
TypeReflectInfo* GetTypeReflectInfo(unsigned int*);
TypeReflectInfo* GetTypeReflectInfo(long*);
TypeReflectInfo* GetTypeReflectInfo(unsigned long*);
TypeReflectInfo* GetTypeReflectInfo(long long*);
TypeReflectInfo* GetTypeReflectInfo(unsigned long long*);
TypeReflectInfo* GetTypeReflectInfo(float*);
TypeReflectInfo* GetTypeReflectInfo(double*);
TypeReflectInfo* GetTypeReflectInfo(STRING*);

#endif