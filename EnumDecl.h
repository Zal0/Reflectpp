#define ENUM_ENTRY1(A) A,
#define ENUM_ENTRY2(A, B) A = B,
enum ENUM_NAME {
	ENUM_ENTRIES
};
#undef ENUM_ENTRY1
#undef ENUM_ENTRY2

#define ENUM_ENTRY1(A) EnumReflectData(#A, (int)A),
#define ENUM_ENTRY2(A, B) EnumReflectData(#A, B),

class CONCAT(ENUM_NAME, ReflectInfo)
{
public:
	static const EnumReflectData* ReflectDatas()
	{
		static const EnumReflectData reflect_datas[] = {ENUM_ENTRIES EnumReflectData("", 0)};
		return reflect_datas;
	}

	static ReflectInfo* DefaultReflectInfo() 
	{
		static ReflectInfo ret(ReflectTypeBySize(sizeof(ENUM_NAME)), "", 0, (PTR)ReflectDatas);
		return &ret;
	}
};

#undef ENUM_ENTRY1
#undef ENUM_ENTRY2

#undef ENUM_NAME
#undef ENUM_ENTRIES