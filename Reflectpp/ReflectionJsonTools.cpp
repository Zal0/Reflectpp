#include "ReflectionJsonTools.h"

static int n_tabs = 0;

void WriteTabs(FILE_OUT out)
{
	for(int i = 0; i < n_tabs; ++i)
		FILE_WRITE_STRING(out, "\t");
}

void NewLine(FILE_OUT out)
{
	FILE_WRITE_CHAR(out, '\n');
	WriteTabs(out);
}

void Comma(FILE_OUT out)
{
	FILE_WRITE_CHAR(out, ',');
	NewLine(out);
}

void OpenBracket(FILE_OUT out, char bracket)
{
	FILE_WRITE_CHAR(out, bracket);
	n_tabs ++;
}

void CloseBracket(FILE_OUT out, char bracket)
{
	n_tabs --;
	NewLine(out);
	FILE_WRITE_CHAR(out, bracket);
}

void Serialize(FILE_OUT out, const ReflectField& reflectable)
{
	if(reflectable.IsArray())
	{
		OpenBracket(out, '[');
		for(int i = 0; i < reflectable.GetNumElems(); ++i)
		{
			if(i == 0) 
			{
				NewLine(out);
			}
			else
			{
				Comma(out);
			}
			Serialize(out, ReflectField(reflectable.GetElem(i)));
		}
		CloseBracket(out, ']');
	}
	else
	{
		switch(reflectable.infos->info->reflect_type)
		{
			case Reflectpp::REFLECT_TYPE_CLASS: {
				OpenBracket(out, '{');
				ReflectInfoIterator it(reflectable.ClassPtr());
				ReflectField info(0,0);
				bool first_field = true;
				while((info = it.Next()).reflectable)
				{
					if(first_field) 
					{
						first_field = false;
						NewLine(out);
					}
					else
					{
						Comma(out);
					}

					FILE_WRITE_STRING(out, "\"");
					for(const char* c = info.infos->id; *c != '[' && *c != '\0'; ++c)
					{
						FILE_WRITE_CHAR(out, *c);
					}
					FILE_WRITE_STRING(out, "\":");
					Serialize(out, info);
				}
				CloseBracket(out, '}');
				break;
			}

			default:
			{
				Reflectpp::Type reflect_type = reflectable.infos->info->reflect_type;
				if(reflect_type == Reflectpp::REFLECT_TYPE_PROPERTY)
				{
					reflect_type = ((TypeReflectInfo*)reflectable.infos->info->extra)->reflect_type;
				}
				
				if(reflect_type == Reflectpp::REFLECT_TYPE_STRING)
				{
					FILE_WRITE_STRING(out, "\"");
					FILE_WRITE_STRING(out, STRING_TO_CHAR_PTR(reflectable.ToString()));
					FILE_WRITE_STRING(out, "\"");
				}
				else
					FILE_WRITE_STRING(out, STRING_TO_CHAR_PTR(reflectable.ToString()));
				break;
			}
		}
	}
}

void Serialize(Reflectable* reflectable, const char* path)
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
	STRING buffer;
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

	STRING& NextToken() 
	{
		STRING_CLEAR(buffer);
		while(IsWhiteSpace()) {Get();}

		if(IsSpecialChar())
		{
			STRING_APPEND(buffer, c);
			Get();
		}
		else if(c == '\"')
		{
			Get();
			while(c != '\"')
			{
				STRING_APPEND(buffer, c);
				Get();
			}
			Get();
		}
		else
		{
			while(!IsWhiteSpace() && !IsSpecialChar())
			{
				STRING_APPEND(buffer, c);
				Get();
			}
		}
		return buffer;
	}

	STRING& ReadBlock(char open_char, char end_char)
	{
		int level = 1;
		do
		{
			Get();
			if(c == open_char)
				level ++;
			if(c == end_char)
				level --;
			STRING_APPEND(buffer, c);
		}
		while(level != 0 || c != end_char);

		Get();

		return buffer;
	}

	void SkipBlock(char open_char, char end_char)
	{
		int level = 1;
		do
		{
			Get();
			if(c == open_char)
				level ++;
			if(c == end_char)
				level --;
		}
		while(level != 0);
		
		Get();
	}
};

void Deserialize(ReflectField& reflectable, PeekStream& in)
{
	STRING& token = in.buffer; // Value
	if(token[0] == '{')
	{
		if(reflectable.reflectable && reflectable.infos->info->reflect_type == Reflectpp::REFLECT_TYPE_CLASS)
		{
			STRING& token = in.NextToken();
			while(token[0] != '}')
			{
				ReflectField r_info = reflectable.Get(STRING_TO_CHAR_PTR(token));
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
		else 
		{
			if(reflectable.reflectable)
				reflectable = STRING_TO_CHAR_PTR(in.ReadBlock('{', '}'));
			else
				in.SkipBlock('{', '}');
		}
	}
	else if(token[0] == '[')
	{
		if(reflectable.reflectable && reflectable.IsArray())
		{
			VectorHandler v;
			if(reflectable.infos->info->reflect_type == Reflectpp::REFLECT_TYPE_VECTOR)
			{
				v = reflectable.GetVectorHandler();
				v->Clear();
			}
		
			token = in.NextToken();
			int elem_idx = 0;
			while(token[0] != ']')
			{
				if(reflectable.infos->info->reflect_type == Reflectpp::REFLECT_TYPE_VECTOR)
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
		else
		{
			if(reflectable.reflectable)
				reflectable = STRING_TO_CHAR_PTR(in.ReadBlock('[', ']'));
			else
				in.SkipBlock('[', ']');
		}
	}
	else if(reflectable.reflectable)
	{
		reflectable = STRING_TO_CHAR_PTR(token);
	}
}

bool Deserialize(Reflectable* reflectable, const char* path)
{
	FILE_IN_LOAD(fin, path);
	if(fin)
	{
		PeekStream p(fin);
		ReflectField rf(reflectable);
		Deserialize(rf, p);
		FILE_IN_CLOSE(fin);
		
		return true;
	}
	return false;
}