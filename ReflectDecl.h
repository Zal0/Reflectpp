//Fields declaration
#undef REFLECT_INT
#undef REFLECT_SHORT
#undef REFLECT_FLOAT
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS

#define REFLECT_INT(ACCESS, A, DEFAULT)    ACCESS: int A;	
#define REFLECT_SHORT(ACCESS, A, DEFAULT)  ACCESS: short A;
#define REFLECT_FLOAT(ACCESS, A, DEFAULT)  ACCESS: float A;
#define REFLECT_CLASS(ACCESS, CLASS, name) ACCESS: CLASS name;
#define REFLECT_VECTOR_CLASS(ACCESS, CLASS, name) ACCESS: std::vector< CLASS > name;
REFLECTION_DATA

//Class ReflectInfos array
#undef REFLECT_INT
#undef REFLECT_SHORT
#undef REFLECT_FLOAT
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS

#define REFLECT_INT(ACCESS, A, DEFAULT)        ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_INT,   #A, OFFSET(A)),
#define REFLECT_SHORT(ACCESS, A, DEFAULT)      ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_SHORT, #A, OFFSET(A)),
#define REFLECT_FLOAT(ACCESS, A, DEFAULT)      ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT, #A, OFFSET(A)),
#define REFLECT_CLASS(ACCESS, CLASS, A)        ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_CLASS, #A, OFFSET(A), (PTR)CLASS::ClassReflectInfos),
#define REFLECT_VECTOR_CLASS(ACCESS, CLASS, A) ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS, #A, OFFSET(A), (PTR)VectorHandlerT< CLASS >::GetVectorHandler),

public:

static ReflectInfo* ClassReflectInfos() {
	static ReflectInfo info[] = {
		ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_INHERITANCE_TABLE, "rht", (PTR)InheritanceTable),
		REFLECTION_DATA
		ReflectInfo::End
	};
	return info;
}
virtual ReflectInfo* ReflectInfos() {return ClassReflectInfos();}

virtual void* This() {return this;}

//Fields initialization
#undef REFLECT_INT
#undef REFLECT_SHORT
#undef REFLECT_FLOAT
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS
	
#define REFLECT_INT(ACCESS, A, DEFAULT)	  A = DEFAULT;
#define REFLECT_SHORT(ACCESS, A, DEFAULT) A = DEFAULT;
#define REFLECT_FLOAT(ACCESS, A, DEFAULT) A = DEFAULT;
#define REFLECT_CLASS(ACCESS, CLASS, A)
#define REFLECT_VECTOR_CLASS(ACCESS, CLASS, A)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
#undef REFLECT_INT
#undef REFLECT_SHORT
#undef REFLECT_FLOAT
#undef REFLECT_CLASS
#undef REFLECT_VECTOR_CLASS
