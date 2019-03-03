//Fields declaration
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME) ACCESS: TYPE NAME;

#ifndef REFLECTION_DATA
	#define REFLECTION_DATA
#endif

REFLECTION_DATA

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3

//Class ReflectInfos array
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ReflectInfo(::DefaultReflectInfo((TYPE*)0)->reflect_type, #NAME, OFFSET(NAME), ::DefaultReflectInfo((TYPE*)0)->extra),
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)          ReflectInfo(::DefaultReflectInfo((TYPE*)0)->reflect_type, #NAME, OFFSET(NAME), ::DefaultReflectInfo((TYPE*)0)->extra),

public:

static ReflectInfo* DefaultReflectInfo()
{
	static ReflectInfo info[] = {
		ReflectInfo(ReflectInfo::REFLECT_TYPE_INHERITANCE_TABLE, "rht", (PTR)InheritanceTable),
		REFLECTION_DATA
		ReflectInfo::End
	};
	static ReflectInfo ret(ReflectInfo::REFLECT_TYPE_CLASS, ReflectableClassName(), 0, (PTR)info);
	return &ret;
}

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3

//Fields initialization
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3

#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
