#include "JsonTools.h"

#include <fstream>

void Serialize(std::ofstream& out, void* reflectable, ReflectInfo* infos)
{
	out << "{";

	ReflectInfoIterator it(reflectable, infos);
	ReflectInfoIterator::Reflectable_Info info(0,0);
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
	std::ofstream fout(path, std::ios::out);
	Serialize(fout, ((Reflectable*)reflectable)->ClassAddress(), ((Reflectable*)reflectable)->ReflectInfos());
	fout.close();
}