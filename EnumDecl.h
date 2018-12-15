#define ENUM_ENTRY(A) A,
#define ENUM_ENTRY_VALUE(A, B) A = B,
enum ENUM_NAME {
	ENUM_ENTRIES
};
#undef ENUM_ENTRY
#undef ENUM_ENTRY_VALUE

#define ENUM_ENTRY(A) EnumReflectData(#A, (int)A),
#define ENUM_ENTRY_VALUE(A, B) EnumReflectData(#A, B),
static const EnumReflectData CONCAT(ENUM_NAME, ReflectDatas)[] = {ENUM_ENTRIES EnumReflectData("", 0)};
#undef ENUM_ENTRY
#undef ENUM_ENTRY_VALUE

#undef ENUM_NAME
#undef ENUM_ENTRIES