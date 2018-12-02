#include "JsonTools.h"

#include <fstream>

void Serialize(std::ofstream& out, void* reflectable, ReflectInfo* infos)
{
	out << "{";

	ReflectInfoIterator it(reflectable, infos);
	ReflectInfoIterator::ReflectField info(0,0);
	bool first_field = true;
	while((info = it.Next()).reflectable)
	{
		if(first_field) 
		{
			first_field = false;
		}
		else
		{
			out << ", ";
		}

		switch (info.infos->reflect_type)
		{
			case ReflectInfo::ReflectType::REFLECT_TYPE_INT:
				out << "\"" << info.infos->id << "\": " << info.Int();
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT:
				out << "\"" << info.infos->id << "\": " << info.Short();
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT:
				out << "\"" << info.infos->id << "\": " << info.Float();
				break;

			case ReflectInfo::ReflectType::REFLECT_TYPE_CLASS: {
				out << "\"" << info.infos->id << "\": ";
				Serialize(out, info.ClassPtr(), info.ReflectInfos());
				break;
			}

			case ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS: {
				VectorHandler vector_handler = info.GetVectorHandler();
				out << "\"" << info.infos->id << "\": [";
				for(int i = 0; i < vector_handler->GetNumElems(); ++i)
				{
					if(i != 0) out << ", ";
					Serialize(out, vector_handler->GetElem(i), vector_handler->GetItemsReflectInfos());
				}
				out << "]";
				break;
			}
		}
	}

	out << "}";
}

void Serialize(void* reflectable, char* path)
{
	std::ofstream fout(path);
	Serialize(fout, ((Reflectable*)reflectable)->This(), ((Reflectable*)reflectable)->ReflectInfos());
	fout.close();
}


bool IsWhiteSpace(const char& c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char* NextToken(char* buffer, std::ifstream& in) 
{
	char* ret = buffer;
	char c;
	do{ in >> c;} while(IsWhiteSpace(c));

	if(c == '\"')
	{
		in >> c;
		while(c != '\"')
		{
			*(buffer ++) = c;
			in >> c;
		}
	}
	else
	{
		while(!IsWhiteSpace(c))
		{
			*(buffer ++) = c;
			in >> c;
		}
	}
	(*buffer) = '\0';
	return ret;
}

void DeserializeValue(ReflectInfoIterator::ReflectField& r_info, char* buffer, std::ifstream& in, void* reflectable, ReflectInfo* infos);
void Deserialize(std::ifstream& in, void* reflectable, ReflectInfo* infos)
{
	char buffer[255];
	NextToken(buffer, in); // key
	while(buffer[0] != '}')
	{
		ReflectInfoIterator::ReflectField r_info = ReflectInfoIterator::ReflectField(reflectable, infos).Get(buffer);
		NextToken(buffer, in); // :

		DeserializeValue(r_info, buffer, in, reflectable, infos);

		NextToken(buffer, in);
	}
}

void DeserializeValue(ReflectInfoIterator::ReflectField& r_info, char* buffer, std::ifstream& in, void* reflectable, ReflectInfo* infos)
{
	NextToken(buffer, in); // Value
	if(buffer[0] == '{')
	{
		Deserialize(in, r_info.ClassPtr(), r_info.ReflectInfos());
	}
	else if(buffer[0] == '[')
	{
		VectorHandler v = r_info.GetVectorHandler();
		NextToken(buffer, in);
		while(buffer[0] != ']')
		{
			v->Push();
			void* new_elem = v->GetElem(v->GetNumElems() - 1);
			DeserializeValue(r_info, buffer, in, new_elem, v->GetItemsReflectInfos());
		}
	}
	else
	{
		switch (r_info.infos->reflect_type)
		{
			case ReflectInfo::ReflectType::REFLECT_TYPE_INT:   r_info.Int()   = atoi(buffer); break;
			case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT: r_info.Short() = (short)atoi(buffer); break;
			case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT: r_info.Float() = (float)atof(buffer); break;
			default: break;
		}
	}
}

void Deserialize(void* reflectable, char* path)
{
	std::ifstream fin(path);
	char tmp[2];
	NextToken(tmp, fin); //Skip the first '{'
	Deserialize(fin, ((Reflectable*)reflectable)->This(), ((Reflectable*)reflectable)->ReflectInfos());
	fin.close();
}