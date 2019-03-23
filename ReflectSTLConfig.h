#ifndef REFLECT_STL_CONFIG_H
#define REFLECT_STL_CONFIG_H

#include <vector>
#include <string.h>

#define VECTOR(TYPE) std::vector< TYPE >
#define VECTOR_SIZE(V) V.size()
#define VECTOR_PUSH(V,i) V.push_back(i)
#define VECTOR_POP(V) V.pop_back()
#define VECTOR_CLEAR(V) V.clear()
#define VECTOR_GET(V, n) V[n]
#define VECTOR_IS_EMPTY(V) V.empty()

#define STRING std::string
#define STRING_TO_CHAR_PTR(STR) STR.c_str()

#endif