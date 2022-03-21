#ifndef PARAMVALUE_H
#define PARAMVALUE_H

#include "iceimagedef.h"

namespace ICEIMAGE_NS
{

template <typename T>
class ParamValue
{
public:
	ParamValue()
	{

	}

	ParamValue(T value)
	{
		m_value = value;
	}

	~ParamValue()
	{

	}

	T& operator=(const T& value)
	{
		m_usedValue = value;
		return m_usedValue;
	}

	void use()
	{
		m_value = m_usedValue;
	}

	bool isUsed() const
	{
		return m_value == m_usedValue;
	}

	const T& getValue() const
	{
		return m_value;
	}

	const T& getUsedValue() const
	{
		return m_usedValue;
	}

private:
	T m_value;
	T m_usedValue;
};

} // namespace ICEIMAGE_NS

#endif // PARAMVALUE_H
