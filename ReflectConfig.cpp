#include "ReflectConfig.h"
#include "Reflection.h"

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