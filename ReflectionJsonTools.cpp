#include "ReflectionJsonTools.h"

#include <fstream>
#include <stdlib.h>

void Serialize(std::ofstream& out, const ReflectField& reflectable)
{
	if(reflectable.IsArray())
	{
		out << "[";
		for(int i = 0; i < reflectable.GetNumElems(); ++i)
		{
			if(i != 0) out << ", ";
			Serialize(out, ReflectField(reflectable.GetElem(i)));
		}
		out << "]";
	}
	else
	{
		switch(reflectable.infos->info->reflect_type)
		{
			case TypeReflectInfo::REFLECT_TYPE_CLASS: {
				out << "{";
				ReflectInfoIterator it(reflectable.ClassPtr());
				ReflectField info(0,0);
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

					out << "\"";
					for(const char* c = info.infos->id; *c != '[' && *c != '\0'; ++c)
					{
						out << *c;
					}
					out << "\"" << ":";
					Serialize(out, info);
				}
				out << "}";
				break;
			}
		
			case TypeReflectInfo::REFLECT_TYPE_STRING:
				out << '\"' << reflectable.ToString().c_str() << '\"';
				break;

			default:
				out << reflectable.ToString().c_str();
				break;
		}
	}
}

void Serialize(Reflectable* reflectable, char* path)
{
	std::ofstream fout(path);
	Serialize(fout, reflectable);
	fout.close();
}

class PeekStream 
{
private:
	void Get() {c = stream.get();}
	char c; //The next token available (already read, is cached here)

public:
	char buffer[255];
	std::ifstream& stream;

	PeekStream(std::ifstream& stream) : stream(stream) {c = stream.get(); NextToken();}

	bool IsWhiteSpace()
	{
		return c == ' ' || c == '\t' || c == '\r' || c == '\n';
	}

	bool IsSpecialChar() 
	{
		return c == '{' || c == '}' || c == ':' || c == ',' || c == '[' || c == ']';
	}

	char* NextToken() 
	{
		char* buf = buffer;
		while(IsWhiteSpace()) {Get();}

		if(IsSpecialChar())
		{
			*(buf ++) = c;
			Get();
		}
		else if(c == '\"')
		{
			Get();
			while(c != '\"')
			{
				*(buf ++) = c;
				Get();
			}
			Get();
		}
		else
		{
			while(!IsWhiteSpace() && !IsSpecialChar())
			{
				*(buf ++) = c;
				Get();
			}
		}
		(*buf) = '\0';
		return buffer;
	}
};

void Deserialize(ReflectField& reflectable, PeekStream& in)
{
	char* token = in.buffer; // Value
	if(token[0] == '{')
	{
		char* token = in.NextToken();
		while(token[0] != '}')
		{
			ReflectField r_info = reflectable.Get(token);
			in.NextToken(); // :

			in.NextToken();
			Deserialize(r_info, in);

			token = in.NextToken(); //,
			if(token[0] == ',')
			{
				token = in.NextToken();
			}
		}
	}
	else if(token[0] == '[')
	{
		VectorHandler v;
		if(reflectable.infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_VECTOR)
		{
			v = reflectable.GetVectorHandler();
			v->Clear();
		}
		
		token = in.NextToken();
		int elem_idx = 0;
		while(token[0] != ']')
		{
			if(reflectable.infos->info->reflect_type == TypeReflectInfo::REFLECT_TYPE_VECTOR)
				v->Push();

			ReflectField new_elem = reflectable.GetElem(elem_idx);
			Deserialize(new_elem, in);

			token = in.NextToken();
			if(token[0] == ',')
			{
				token = in.NextToken();
			}

			elem_idx ++;
		}
	}
	else if(reflectable.reflectable)
	{
		reflectable = token;
	}
}

void Deserialize(Reflectable* reflectable, char* path)
{
	std::ifstream fin(path, std::ios::binary);
	PeekStream p(fin);
	ReflectField rf(reflectable);
	Deserialize(rf, p);
	fin.close();
}