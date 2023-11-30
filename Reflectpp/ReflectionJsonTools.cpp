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

void Serialize(Reflectable* reflectable, FILE_OUT out)
{
	ReflectField rf(reflectable);
	Serialize(out, rf);
}

void Serialize(Reflectable* reflectable, const char* path)
{
	FILE_OUT_LOAD(fout, path);
	Serialize(reflectable, fout);
	FILE_OUT_CLOSE(fout);
}


//---------- PEEKSTREAM ------------------------------------------------------------------------------
	void PeekStream::Get()
	{
		if(depth)
			c = FILE_READ_CHAR(stream);
		else
			c = ' ';

		if(c == '{')
			depth ++;
		else if(c == '}')
			depth --;
	}

	bool PeekStream::IsWhiteSpace()
	{
		return c == ' ' || c == '\t' || c == '\r' || c == '\n';
	}

	bool PeekStream::IsSpecialChar()
	{
		return c == '{' || c == '}' || c == ':' || c == ',' || c == '[' || c == ']';
	}

	PeekStream::PeekStream(FILE_IN stream) : stream(stream)
	{
		depth = -1;
		c = FILE_READ_CHAR(stream); 
		NextToken();
		depth = 1;
	}

	STRING& PeekStream::NextToken()
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

	STRING& PeekStream::ReadBlock(char open_char, char end_char)
	{
		int level = 1;
		while(true)
		{
			if(c == open_char)
				level ++;
			if(c == end_char)
				level --;
			STRING_APPEND(buffer, c);

			if(level == 0)
				break;

			Get();
		}

		Get();

		return buffer;
	}

	void PeekStream::SkipBlock(char open_char, char end_char)
	{
		int level = 1;
		while(true)
		{
			if(c == open_char)
				level ++;
			if(c == end_char)
				level --;

			if(level == 0)
				break;

			Get();
		}
		
		Get();
	}
//----------------------------------------------------------------------------------------------------

void Deserialize(ReflectField& reflectable, PeekStream& in)
{
	STRING& token = in.buffer; // Value
	if(token == "{")
	{
		if(reflectable.reflectable && reflectable.infos->info->reflect_type == Reflectpp::REFLECT_TYPE_CLASS)
		{
			STRING& token = in.NextToken();
			while(token != "}")
			{
				ReflectField r_info = reflectable.Get(STRING_TO_CHAR_PTR(token));
				in.NextToken(); // :

				in.NextToken();
				Deserialize(r_info, in);

				token = in.NextToken(); //,
				if(token == ",")
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
	else if(token == "[")
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
			while(token != "]")
			{
				if(reflectable.infos->info->reflect_type == Reflectpp::REFLECT_TYPE_VECTOR)
					v->Push();

				ReflectField new_elem = reflectable.GetElem(elem_idx);
				Deserialize(new_elem, in);

				token = in.NextToken();
				if(token == ",")
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

void Deserialize(Reflectable* reflectable, FILE_IN in)
{
	if(FILE_IN_EOF(in))
		return;

	PeekStream p(in);
	ReflectField rf(reflectable);
	Deserialize(rf, p);
}

bool Deserialize(Reflectable* reflectable, const char* path)
{
	FILE_IN_LOAD(fin, path);
	if(fin)
	{
		Deserialize(reflectable, fin);
		FILE_IN_CLOSE(fin);
		
		return true;
	}
	return false;
}