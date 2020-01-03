//Fields declaration
#define EXPOSED_FIELD(TYPE, NAME)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ACCESS: TYPE NAME;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME) ACCESS: TYPE NAME;
#define SERIALIZED_PROPERTY(TYPE, NAME, GETTER, SETTER)

#ifndef REFLECTION_DATA
	#define REFLECTION_DATA
#endif

REFLECTION_DATA

#undef EXPOSED_FIELD
#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef SERIALIZED_PROPERTY

//Class ReflectInfos array
#define EXPOSED_FIELD(TYPE, NAME) SERIALIZED_FIELD(0, TYPE, NAME, 0)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) ReflectInfo(::GetTypeReflectInfo((TYPE*)0), #NAME, OFFSET(NAME)),
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)          ReflectInfo(::GetTypeReflectInfo((TYPE*)0), #NAME, OFFSET(NAME)),
#define SERIALIZED_PROPERTY(TYPE, NAME, GETTER, SETTER)ReflectInfo(PropertyReflectInfo< TYPE >(), #NAME, (PTR)&(Prop##NAME())),

public:

static TypeReflectInfo* GetTypeReflectInfo()
{
	static ReflectInfo info[] = {
		ReflectInfo(&TypeReflectInfo::InheritanceTable, "rht", (PTR)InheritanceTable),
		REFLECTION_DATA
		REFLECTINFO_END
	};
	static TypeReflectInfo t_info(Reflectpp::REFLECT_TYPE_CLASS, sizeof(*ReflectClass()), (PTR)info);
	return &t_info;
}

#undef EXPOSED_FIELD
#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef SERIALIZED_PROPERTY

//Fields initialization
#define EXPOSED_FIELD(TYPE, NAME)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT) NAME = DEFAULT;
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)
#define SERIALIZED_PROPERTY(TYPE, NAME, GETTER, SETTER)
void ReflectInit()
{
	REFLECTION_DATA
}

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef EXPOSED_FIELD
#undef SERIALIZED_PROPERTY

//Properties
#define EXPOSED_FIELD(TYPE, NAME)
#define SERIALIZED_FIELD4(ACCESS, TYPE, NAME, DEFAULT)
#define SERIALIZED_FIELD3(ACCESS, TYPE, NAME)
#define SERIALIZED_PROPERTY(TYPE, NAME, GETTER, SETTER)                              \
	static Property< CLASS_NAME, TYPE >& Prop##NAME(){                                 \
		static Property<CLASS_NAME, TYPE> ret(&CLASS_NAME::GETTER, &CLASS_NAME::SETTER); \
		return ret;                                                                      \
	}

REFLECTION_DATA

#undef SERIALIZED_FIELD4
#undef SERIALIZED_FIELD3
#undef EXPOSED_FIELD
#undef SERIALIZED_PROPERTY


#undef REFLECTION_DATA
#undef REFLECTION_INHERITANCE_DATA
