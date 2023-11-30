#ifndef REFLECT_PROPERTY
#define REFLECT_PROPERTY

class PropertyI
{
public:
	virtual void Get(void* t, void* ret) = 0;
	virtual void Set(void* t, void* val) = 0;
};

template< class T, class V >
class Property : public PropertyI
{
private:
	V(T::* getter)() const;
	void(T::* setter)(const V& v);

public:
	Property(V(T::* getter)() const, void(T::* setter)(const V& v)) : getter(getter), setter(setter) {}

	void Get(void* t, void* ret) { *((V*)ret) = (((T*)t)->*getter)(); }
	void Set(void* t, void* val) { (((T*)t)->*setter)(*(V*)val); }
};

#endif