//Fields declaration
#define EXPOSED_FIELD(TYPE, NAME)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME) ACCESS: TYPE NAME;

#ifndef REFLECTION_DATA
	#define REFLECTION_DATA
#endif

REFLECTION_DATA

#undef EXPOSED_FIELD
#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3

//Class ReflectInfos array
#define EXPOSED_FIELD(TYPE, NAME) SERIALIZED_FIELD(0, TYPE, NAME, 0)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ReflectInfo(::DefaultReflectInfo((TYPE*)0)->info, #NAME, OFFSET(NAME)),
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)          ReflectInfo(::DefaultReflectInfo((TYPE*)0)->info, #NAME, OFFSET(NAME)),

public:

static ReflectInfo* DefaultReflectInfo()
{
	static ReflectInfo info[] = {
		ReflectInfo(&TypeReflectInfo::InheritanceTable, "rht", (PTR)InheritanceTable),
		REFLECTION_DATA
		ReflectInfo::End
	};
	static TypeReflectInfo t_info(TypeReflectInfo::REFLECT_TYPE_CLASS, sizeof(*ReflectClass()), (PTR)info);
	static ReflectInfo ret(&t_info, ReflectableClassName(), 0);
	return &ret;
}

#undef EXPOSED_FIELD
#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3

//Fields initialization
#define EXPOSED_FIELD(TYPE, NAME)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef EXPOSED_FIELD

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
