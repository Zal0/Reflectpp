#define ENUM_ENTRY1(A) A,
#define ENUM_ENTRY2(A, B) A = B,
#define ENUM_NAME_ CONCAT(ENUM_NAME,_)

enum ENUM_NAME_ {
	ENUM_ENTRIES
};

class ENUM_NAME
{
private:
	ENUM_NAME_ value;

public:
	ENUM_NAME()  {}
	ENUM_NAME(int val) : value((ENUM_NAME_)val) {}
	ENUM_NAME(ENUM_NAME_ val) : value(val) {}
	operator ENUM_NAME_() const {return value;}

#undef ENUM_ENTRY1
#undef ENUM_ENTRY2

#define ENUM_ENTRY1(A) EnumReflectData(#A, (int)A),
#define ENUM_ENTRY2(A, B) EnumReflectData(#A, B),
	static const EnumReflectData* ReflectDatas()
	{
		static const EnumReflectData reflect_datas[] = {ENUM_ENTRIES EnumReflectData("?", 0)};
		return reflect_datas;
	}

	static ReflectInfo* DefaultReflectInfo() 
	{
		static TypeReflectInfo t_info(ReflectTypeBySize(sizeof(ENUM_NAME)), sizeof(ENUM_NAME), (PTR)(ReflectDatas()));
		static ReflectInfo ret(&t_info, "", 0);
		return &ret;
	}
};

#undef ENUM_ENTRY1
#undef ENUM_ENTRY2

#undef _ENUM
#undef ENUM_NAME
#undef ENUM_ENTRIES