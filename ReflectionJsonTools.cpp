#include "ReflectionJsonTools.h"

void Serialize(FILE_OUT out, const ReflectField& reflectable)
{
	if(reflectable.IsArray())
	{
		FILE_WRITE_STRING(out, "[");
		for(int i = 0; i < reflectable.GetNumElems(); ++i)
		{
			if(i != 0) FILE_WRITE_STRING(out, ", ");
			Serialize(out, ReflectField(reflectable.GetElem(i)));
		}
		FILE_WRITE_STRING(out, "]");
	}
	else
	{
		switch(reflectable.infos->info->reflect_type)
		{
			case TypeReflectInfo::REFLECT_TYPE_CLASS: {
				FILE_WRITE_STRING(out, "{");
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
						FILE_WRITE_STRING(out, ", ");
					}

					FILE_WRITE_STRING(out, "\"");
					for(const char* c = info.infos->id; *c != '[' && *c != '\0'; ++c)
					{
						FILE_WRITE_CHAR(out, *c);
					}
					FILE_WRITE_STRING(out, "\":");
					Serialize(out, info);
				}
				FILE_WRITE_STRING(out, "}");
				break;
			}
		
			case TypeReflectInfo::REFLECT_TYPE_STRING:
				FILE_WRITE_STRING(out, "\"");
				FILE_WRITE_STRING(out, STRING_TO_CHAR_PTR(reflectable.ToString()));
				FILE_WRITE_STRING(out, "\"");
				break;

			default:
				FILE_WRITE_STRING(out, STRING_TO_CHAR_PTR(reflectable.ToString()));
				break;
		}
	}
}

void Serialize(Reflectable* reflectable, char* path)
{
	FILE_OUT_LOAD(fout, path);
	Serialize(fout, reflectable);
	FILE_OUT_CLOSE(fout);
}

class PeekStream 
{
private:
	void Get() {c = FILE_READ_CHAR(stream);}
	char c; //The next token available (already read, is cached here)

public:
	char buffer[255];
	FILE_IN stream;

	PeekStream(FILE_IN stream) : stream(stream) {c = FILE_READ_CHAR(stream); NextToken();}

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
	FILE_IN_LOAD(fin, path);
	PeekStream p(fin);
	ReflectField rf(reflectable);
	Deserialize(rf, p);
	FILE_IN_CLOSE(fin);
}