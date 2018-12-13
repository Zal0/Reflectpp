//Fields declaration
#undef SERIALIZED_FIELD
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS

#define SERIALIZED_FIELD(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define REFLECT_CLASS(ACCESS, CLASS, name) ACCESS: CLASS name;
#define REFLECT_VECTOR_CLASS(ACCESS, CLASS, name) ACCESS: std::vector< CLASS > name;
REFLECTION_DATA

//Class ReflectInfos array
#undef SERIALIZED_FIELD
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS

#define SERIALIZED_FIELD(ACCESS, TYPE, NAME, DEFAULT) ReflectInfo(DefaultReflectInfo< TYPE >()->reflect_type, #NAME, OFFSET(NAME)),
#define REFLECT_CLASS(ACCESS, CLASS, A)        ReflectInfo(ReflectInfo::REFLECT_TYPE_CLASS, #A, OFFSET(A), (PTR)CLASS::ClassReflectInfos),
#define REFLECT_VECTOR_CLASS(ACCESS, CLASS, A) ReflectInfo(ReflectInfo::REFLECT_TYPE_VECTOR_CLASS, #A, OFFSET(A), (PTR)VectorHandlerT< CLASS >::GetVectorHandler),

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
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS

#define SERIALIZED_FIELD(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define REFLECT_CLASS(ACCESS, CLASS, A)
#define REFLECT_VECTOR_CLASS(ACCESS, CLASS, A)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
#undef SERIALIZED_FIELD
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS
