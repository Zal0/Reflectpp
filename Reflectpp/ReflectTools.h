#ifndef REFLECTTOOLS_H
#define REFLECTTOOLS_H

#if defined(_WIN64) || defined(__aarch64__) || defined(__x86_64__)
typedef long long PTR;
#else
typedef int PTR;
#endif

#define DUMMY_ADDRESS 0x10000

//Calculates the offset of a member inside a class
#define OFFSET(FIELD) ((PTR)&((ReflectClass())->FIELD) - (PTR)(ReflectClass()))

//Calculates the offset of a class when using inheritance (note: this doesn't work if the adrees is 0x0)
#define CLASS_OFFSET(CLASS) ((PTR)((CLASS*)ReflectClass()) - (PTR)(ReflectClass()))

//Returns a pointer casted to TYPE of the data stored in INSTANCE at OFFSET
#define REFLECT_PTR(TYPE, INSTANCE, OFFSET) (TYPE*)(((char*)(INSTANCE)) + OFFSET)

#define CONCAT2(A, B)  A##B
#define CONCAT(A, B) CONCAT2(A, B)

//from https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define EXPAND( x ) x //VS workaround
#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define SERIALIZED_FIELD(...) EXPAND(GET_MACRO(__VA_ARGS__, SERIALIZED_FIELD4, SERIALIZED_FIELD3, SERIALIZED_FIELD2)(__VA_ARGS__))
#define GET_MACRO_ENUM_ENTRY(_1,_2,NAME,...) NAME
#define ENUM_ENTRY(...) EXPAND(GET_MACRO_ENUM_ENTRY(__VA_ARGS__, ENUM_ENTRY2, ENUM_ENTRY1)(__VA_ARGS__))

#define REFLECTINFO_END ReflectInfo(0, "", 0)

#endif