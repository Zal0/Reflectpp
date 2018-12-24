//Fields declaration
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME) ACCESS: TYPE NAME;
#define SERIALIZED_FIELD_VECTOR(ACCESS, TYPE, NAME) ACCESS: std::vector< TYPE > NAME;
#define SERIALIZED_ENUM(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_ENUM_VECTOR(ACCESS, TYPE, NAME) ACCESS: std::vector< TYPE > NAME;

REFLECTION_DATA

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef SERIALIZED_FIELD_VECTOR
#undef SERIALIZED_ENUM
#undef SERIALIZED_ENUM_VECTOR

//Class ReflectInfos array
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ReflectInfo(::DefaultReflectInfo((TYPE*)0)->reflect_type, #NAME, OFFSET(NAME)),
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)          ReflectInfo(::DefaultReflectInfo((TYPE*)0)->reflect_type, #NAME, OFFSET(NAME), ::DefaultReflectInfo((TYPE*)0)->extra),
#define SERIALIZED_ENUM(ACCESS, TYPE, NAME, DEFAULT)   ReflectInfo(ReflectTypeBySize(sizeof(TYPE)), #NAME, OFFSET(NAME), (PTR)(CONCAT(TYPE, ReflectInfo)::ReflectDatas())),
#define SERIALIZED_ENUM_VECTOR(ACCESS, TYPE, NAME)     ReflectInfo(ReflectInfo::REFLECT_TYPE_VECTOR_CLASS, #NAME, OFFSET(NAME), (PTR)VectorHandlerT< TYPE, CONCAT(TYPE, ReflectInfo) >::GetVectorHandler),

public:

static ReflectInfo* ClassReflectInfos() {
	static ReflectInfo info[] = {
		ReflectInfo(ReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE, "rht", (PTR)InheritanceTable),
		REFLECTION_DATA
		ReflectInfo::End
	};
	return info;
}
virtual ReflectInfosFunc ReflectInfosF(){return ClassReflectInfos;}

virtual void* This() {return this;}

static ReflectInfo* DefaultReflectInfo()
{
	static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_CLASS, "", 0, (PTR)(ClassReflectInfos()));
	return &ret;
}

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef SERIALIZED_FIELD_VECTOR
#undef SERIALIZED_ENUM
#undef SERIALIZED_ENUM_VECTOR

//Fields initialization
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)
#define SERIALIZED_FIELD_VECTOR(ACCESS, TYPE, NAME)
#define SERIALIZED_ENUM(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_ENUM_VECTOR(ACCESS, TYPE, NAME)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef SERIALIZED_FIELD_VECTOR
#undef SERIALIZED_ENUM
#undef SERIALIZED_ENUM_VECTOR

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
