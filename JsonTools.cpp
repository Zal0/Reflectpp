#include "JsonTools.h"

#include <fstream>
#include <stdlib.h>

void Serialize(std::ofstream& out, const ReflectField& reflectable)
{
	switch(reflectable.infos->reflect_type)
	{
		case ReflectInfo::REFLECT_TYPE_CLASS: {
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

				out << "\"" << info.infos->id << "\"" << ":";
				Serialize(out, info);
			}
			out << "}";
			break;
		}
			
		case ReflectInfo::REFLECT_TYPE_VECTOR: {
			out << "[";
			VectorHandler vector_handler = reflectable.GetVectorHandler();
			for(int i = 0; i < vector_handler->GetNumElems(); ++i)
			{
				if(i != 0) out << ", ";
				Serialize(out, ReflectField(vector_handler->GetElem(i)));
			}
			out << "]";
			break;
		}
		
		case ReflectInfo::REFLECT_TYPE_STRING:
			out << '\"' << reflectable.ToString().c_str() << '\"';
			break;

		default:
			out << reflectable.ToString().c_str();
			break;
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
		VectorHandler v = reflectable.GetVectorHandler();
		v->Clear();
		token = in.NextToken();
		while(token[0] != ']')
		{
			v->Push();
			ReflectField new_elem = v->GetElem(v->GetNumElems() - 1);
			Deserialize(new_elem, in);

			token = in.NextToken();
			if(token[0] == ',')
			{
				token = in.NextToken();
			}
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