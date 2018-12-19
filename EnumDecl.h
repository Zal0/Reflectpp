#define ENUM_ENTRY1(A) A,
#define ENUM_ENTRY2(A, B) A = B,
enum ENUM_NAME {
	ENUM_ENTRIES
};
#undef ENUM_ENTRY1
#undef ENUM_ENTRY2

#define ENUM_ENTRY1(A) EnumReflectData(#A, (int)A),
#define ENUM_ENTRY2(A, B) EnumReflectData(#A, B),
static const EnumReflectData CONCAT(ENUM_NAME, ReflectDatas)[] = {ENUM_ENTRIES EnumReflectData("", 0)};
#undef ENUM_ENTRY1
#undef ENUM_ENTRY2

#undef ENUM_NAME
#undef ENUM_ENTRIES