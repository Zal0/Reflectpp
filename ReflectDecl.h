//Fields declaration
#undef SERIALIZED_FIELD
#undef SERIALIZED_ENUM
#undef SERIALIZED_FIELD_CLASS
#undef SERIALIZED_FIELD_VECTOR

#define SERIALIZED_FIELD(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_ENUM(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_FIELD_CLASS(ACCESS, CLASS, name) ACCESS: CLASS name;
#define SERIALIZED_FIELD_VECTOR(ACCESS, CLASS, name) ACCESS: std::vector< CLASS > name;
REFLECTION_DATA

//Class ReflectInfos array
#undef SERIALIZED_FIELD
#undef SERIALIZED_ENUM
#undef SERIALIZED_FIELD_CLASS
#undef SERIALIZED_FIELD_VECTOR

#define SERIALIZED_FIELD(ACCESS, TYPE, NAME, DEFAULT) ReflectInfo(DefaultReflectInfo< TYPE >()->reflect_type, #NAME, OFFSET(NAME)),
#define SERIALIZED_ENUM(ACCESS, TYPE, NAME, DEFAULT)  ReflectInfo(ReflectTypeBySize(sizeof(TYPE)), #NAME, OFFSET(NAME), (PTR)CONCAT(TYPE, ReflectDatas)),
#define SERIALIZED_FIELD_CLASS(ACCESS, CLASS, A)      ReflectInfo(ReflectInfo::REFLECT_TYPE_CLASS, #A, OFFSET(A), (PTR)CLASS::ClassReflectInfos),
#define SERIALIZED_FIELD_VECTOR(ACCESS, CLASS, A)     ReflectInfo(ReflectInfo::REFLECT_TYPE_VECTOR_CLASS, #A, OFFSET(A), (PTR)VectorHandlerT< CLASS >::GetVectorHandler),

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

//Fields initialization
#undef SERIALIZED_FIELD
#undef SERIALIZED_ENUM
#undef SERIALIZED_FIELD_CLASS
#undef SERIALIZED_FIELD_VECTOR

#define SERIALIZED_FIELD(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_ENUM(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_FIELD_CLASS(ACCESS, CLASS, A)
#define SERIALIZED_FIELD_VECTOR(ACCESS, CLASS, A)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
#undef SERIALIZED_FIELD
#undef SERIALIZED_FIELD_CLASS
#undef SERIALIZED_FIELD_VECTOR
