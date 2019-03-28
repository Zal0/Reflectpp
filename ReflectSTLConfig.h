#ifndef REFLECT_STL_CONFIG_H
#define REFLECT_STL_CONFIG_H

#include <vector>
#include <string>
#include <memory>
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

#define SMART_PTR(A) std::auto_ptr< A >

#define FILE_OUT std::ofstream&
#define FILE_OUT_LOAD(FILE,PATH) std::ofstream FILE(PATH)
#define FILE_WRITE_STRING(FILE,STR) FILE << STR
#define FILE_WRITE_CHAR(FILE,C) FILE << C
#define FILE_OUT_CLOSE(FILE) FILE.close()

#define FILE_IN std::ifstream&
#define FILE_IN_LOAD(FILE, PATH) std::ifstream FILE(PATH, std::ios::binary)
#define FILE_READ_CHAR(FILE) FILE.get()
#define FILE_IN_CLOSE(FILE) FILE.close()

#endif