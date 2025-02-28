#pragma once

class Base_Condition
{
public:

	virtual ~Base_Condition() = default;
	virtual bool Try() const = 0;
};

template<class T>
class Condition : public Base_Condition
{
public:

	Condition(const T& aFunctor);
	virtual ~Condition() = default;

	bool Try() const override;

	bool operator()() const;

private:

	const T myFunctor;
};

template<class T>
inline Condition<T>::Condition(const T& aFunctor)
	: myFunctor(aFunctor)
{
}

template<class T>
inline bool Condition<T>::Try() const
{
	return (*this)();
}

template<class T>
bool Condition<T>::operator()() const
{
	return myFunctor();
}