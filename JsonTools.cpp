#include "JsonTools.h"

#include <fstream>

void Serialize(std::ofstream& out, ReflectField reflectable)
{
	switch(reflectable.infos->reflect_type)
	{
		case ReflectInfo::ReflectType::REFLECT_TYPE_CLASS: {
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
			
		case ReflectInfo::ReflectType::REFLECT_TYPE_VECTOR_CLASS: {
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

		case ReflectInfo::ReflectType::REFLECT_TYPE_INT:
			out << reflectable.Int();
			break;

		case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT:
			out << reflectable.Short();
			break;

		case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT:
			out << reflectable.Float();
			break;
	}
}

void Serialize(Reflectable* reflectable, char* path)
{
	ReflectField r(reflectable->This(), &ReflectInfo(ReflectInfo::ReflectType::REFLECT_TYPE_CLASS, "", 0, (PTR)(reflectable->ReflectInfosF())));

	std::ofstream fout(path);
	Serialize(fout, r);
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

void DeserializeValue(ReflectField r_info, PeekStream& in);
void Deserialize(ReflectField reflectable, PeekStream& in)
{
	char* token = in.NextToken();
	while(token[0] != '}')
	{
		ReflectField r_info = reflectable.Get(token);
		in.NextToken(); // :

		in.NextToken();
		DeserializeValue(r_info, in);

		token = in.NextToken(); //,
		if(token[0] == ',')
		{
			token = in.NextToken();
		}
	}
}

void DeserializeValue(ReflectField r_info, PeekStream& in)
{
	char* token = in.buffer; // Value
	if(token[0] == '{')
	{
		Deserialize(r_info, in);
	}
	else if(token[0] == '[')
	{
		VectorHandler v = r_info.GetVectorHandler();
		v->Clear();
		token = in.NextToken();
		while(token[0] != ']')
		{
			v->Push();
			ReflectField new_elem = v->GetElem(v->GetNumElems() - 1);
			DeserializeValue(new_elem, in);

			token = in.NextToken();
			if(token[0] == ',')
			{
				token = in.NextToken();
			}
		}
	}
	else if(r_info.reflectable)
	{
		switch (r_info.infos->reflect_type)
		{
			case ReflectInfo::ReflectType::REFLECT_TYPE_INT:   r_info.Int()   = atoi(token); break;
			case ReflectInfo::ReflectType::REFLECT_TYPE_SHORT: r_info.Short() = (short)atoi(token); break;
			case ReflectInfo::ReflectType::REFLECT_TYPE_FLOAT: r_info.Float() = (float)atof(token); break;
			default: break;
		}
	}
}

void Deserialize(Reflectable* reflectable, char* path)
{
	std::ifstream fin(path, std::ios::binary);
	
	DeserializeValue(reflectable, PeekStream(fin));
	fin.close();
}