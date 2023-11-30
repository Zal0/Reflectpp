#ifndef VECTORHANDLER_H
#define VECTORHANDLER_H

#include "ReflectField.h"

class VectorHandlerI
{
public:
	virtual ~VectorHandlerI() {}
	virtual int GetNumElems() = 0;
	virtual void Push() = 0;
	virtual void Pop() = 0;
	virtual void Clear() = 0;
	ReflectField GetElem(int idx) { return ReflectField(GetElemPtr(idx), GetItemsReflectInfos()); }

protected:
	virtual void* GetElemPtr(int idx) = 0;
	virtual ReflectInfo* GetItemsReflectInfos() = 0;
};

template< class T >
class VectorHandlerT : public VectorHandlerI
{
private:
	VECTOR(T)& v;
	VectorHandlerT(void* ptr) : v(*((VECTOR(T)*)ptr)) {}

public:
	static VectorHandler GetVectorHandler(void* ptr) { return VectorHandler(new VectorHandlerT< T >(ptr)); }

	virtual int GetNumElems() { return (int)VECTOR_SIZE(v); }
	virtual void Push() { VECTOR_PUSH(v, T()); }
	virtual void Pop() { VECTOR_POP(v); }
	virtual void Clear() { VECTOR_CLEAR(v); }

protected:
	virtual void* GetElemPtr(int idx) { return &VECTOR_GET(v, idx); }
	virtual ReflectInfo* GetItemsReflectInfos() { static ReflectInfo r_info(GetTypeReflectInfo((T*)0), "", 0); return &r_info; }
};

#endif