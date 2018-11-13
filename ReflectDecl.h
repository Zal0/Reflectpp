#define REFLECT_INT(A) int A;	
#define REFLECT_SHORT(A) short A;
#define REFLECT_FLOAT(A) float A;
#define REFLECT_CLASS(CLASS, name) CLASS name;
REFLECTION_DATA
#undef REFLECT_INT
#undef REFLECT_SHORT
#undef REFLECT_FLOAT
#undef REFLECT_CLASS
	
#define REFLECT_INT(A)	        ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_INT,   #A, OFFSET(A)),
#define REFLECT_SHORT(A)        ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_SHORT, #A, OFFSET(A)),
#define REFLECT_FLOAT(A)        ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT, #A, OFFSET(A)),
#define REFLECT_CLASS(CLASS, A) ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_CLASS, #A, OFFSET(A)),

static ReflectInfo* ClassReflectInfos() {
	static ReflectInfo info[] = {
		ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_INHERITANCE_TABLE, "rht", (PTR)InheritanceTable),
		REFLECTION_DATA
		ReflectInfo::End
	};
	return info;
}
virtual ReflectInfo* ReflectInfos() {return ClassReflectInfos();}

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
#undef REFLECT_INT
#undef REFLECT_SHORT
#undef REFLECT_FLOAT
#undef REFLECT_CLASS