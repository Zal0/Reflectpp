#include "ReflectConfig.h"
#include "Reflection.h"

#define DEF_INFO(TYPE, RTYPE) static TypeReflectInfo t_info(RTYPE, sizeof(TYPE), 0); return &t_info;

TypeReflectInfo* GetTypeReflectInfo(bool*)               {DEF_INFO(bool,               Reflectpp::REFLECT_TYPE_BOOL)}
TypeReflectInfo* GetTypeReflectInfo(char*)               {DEF_INFO(char,               Reflectpp::REFLECT_TYPE_CHAR)}
TypeReflectInfo* GetTypeReflectInfo(unsigned char*)      {DEF_INFO(unsigned char,      Reflectpp::REFLECT_TYPE_UCHAR)}
TypeReflectInfo* GetTypeReflectInfo(short*)              {DEF_INFO(short,              Reflectpp::REFLECT_TYPE_SHORT)}
TypeReflectInfo* GetTypeReflectInfo(unsigned short*)     {DEF_INFO(unsigned short,     Reflectpp::REFLECT_TYPE_USHORT)}
TypeReflectInfo* GetTypeReflectInfo(int*)                {DEF_INFO(int,                Reflectpp::REFLECT_TYPE_INT)}
TypeReflectInfo* GetTypeReflectInfo(unsigned int*)       {DEF_INFO(unsigned int,       Reflectpp::REFLECT_TYPE_UINT)}
TypeReflectInfo* GetTypeReflectInfo(long*)               {DEF_INFO(long,               Reflectpp::REFLECT_TYPE_LONG)}
TypeReflectInfo* GetTypeReflectInfo(unsigned long*)      {DEF_INFO(unsigned long,      Reflectpp::REFLECT_TYPE_ULONG)}
TypeReflectInfo* GetTypeReflectInfo(long long*)          {DEF_INFO(long long,          Reflectpp::REFLECT_TYPE_LONGLONG)}
TypeReflectInfo* GetTypeReflectInfo(unsigned long long*) {DEF_INFO(unsigned long long, Reflectpp::REFLECT_TYPE_ULONGLONG)}
TypeReflectInfo* GetTypeReflectInfo(float*)              {DEF_INFO(float,              Reflectpp::REFLECT_TYPE_FLOAT)}
TypeReflectInfo* GetTypeReflectInfo(double*)             {DEF_INFO(double,             Reflectpp::REFLECT_TYPE_DOUBLE)}
TypeReflectInfo* GetTypeReflectInfo(STRING*)             {DEF_INFO(STRING,             Reflectpp::REFLECT_TYPE_STRING)}

void FromString(ReflectField& field, const Reflectpp::Type reflect_type, const char* str)
{
	switch(reflect_type)
	{
		case Reflectpp::REFLECT_TYPE_BOOL:      field.Set< bool >              (str[0] == '0' ? false : true);   break;
		case Reflectpp::REFLECT_TYPE_CHAR:      field.Set< char >              ((char)atoi(str));                break;
		case Reflectpp::REFLECT_TYPE_UCHAR:     field.Set< unsigned char >     ((unsigned char)atoi(str));       break;
		case Reflectpp::REFLECT_TYPE_SHORT:     field.Set< short >             ((short)atoi(str));               break;
		case Reflectpp::REFLECT_TYPE_USHORT:    field.Set< unsigned short >    ((unsigned short)atoi(str));      break;
		case Reflectpp::REFLECT_TYPE_INT:       field.Set< int >               ((int)atoi(str));                 break;
		case Reflectpp::REFLECT_TYPE_UINT:      field.Set< unsigned int >      ((unsigned int)atoi(str));        break;
		case Reflectpp::REFLECT_TYPE_LONG:      field.Set< long >              ((long)atol(str));                break;
		case Reflectpp::REFLECT_TYPE_ULONG:     field.Set< unsigned long >     ((unsigned long)atol(str));       break;
		case Reflectpp::REFLECT_TYPE_LONGLONG:  field.Set< long long >         ((long long)atoll(str));          break;
		case Reflectpp::REFLECT_TYPE_ULONGLONG: field.Set< unsigned long long >((unsigned long long)atoll(str)); break;
		case Reflectpp::REFLECT_TYPE_FLOAT:     field.Set< float >             ((float)atof(str));               break;
		case Reflectpp::REFLECT_TYPE_DOUBLE:    field.Set< double >            ((double)atof(str));              break;
		case Reflectpp::REFLECT_TYPE_STRING:    field.Set< STRING >            (str);                            break;
		default: break;
	}
}

#define TO_STRING(STR, TYPE) snprintf(buff, BUFF_SIZE, STR, field.Get< TYPE >());
STRING ToString(const ReflectField& field, const Reflectpp::Type reflect_type)
{
	static const int BUFF_SIZE = 50;
	static char buff[BUFF_SIZE];

	switch(reflect_type)
	{
		case Reflectpp::REFLECT_TYPE_BOOL:      TO_STRING("%d", bool);                 break;
		case Reflectpp::REFLECT_TYPE_CHAR:      TO_STRING("%c", char);                 break;
		case Reflectpp::REFLECT_TYPE_UCHAR:     TO_STRING("%u", unsigned char);        break;
		case Reflectpp::REFLECT_TYPE_SHORT:     TO_STRING("%hd", short);               break;
		case Reflectpp::REFLECT_TYPE_USHORT:    TO_STRING("%hu", unsigned short);      break;
		case Reflectpp::REFLECT_TYPE_INT:       TO_STRING("%d", int);                  break;
		case Reflectpp::REFLECT_TYPE_UINT:      TO_STRING("%u", unsigned int);         break;
		case Reflectpp::REFLECT_TYPE_LONG:      TO_STRING("%ld", long);                break;
		case Reflectpp::REFLECT_TYPE_ULONG:     TO_STRING("%lu", unsigned long);       break;
		case Reflectpp::REFLECT_TYPE_LONGLONG:  TO_STRING("%lld", long long);          break;
		case Reflectpp::REFLECT_TYPE_ULONGLONG: TO_STRING("%llu", unsigned long long); break;
		case Reflectpp::REFLECT_TYPE_FLOAT:     TO_STRING("%g", float);                break;
		case Reflectpp::REFLECT_TYPE_DOUBLE:    TO_STRING("%g", double);              break;
		case Reflectpp::REFLECT_TYPE_STRING:    return field.Get< STRING >();
		default: break;
	}

	return STRING(buff);
}