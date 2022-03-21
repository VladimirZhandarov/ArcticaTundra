#include "param.h"

namespace ICEFINDOBJECTPROC_NS
{

Param::Param(QObject* parent) : QObject(parent)
{
	m_maskUsed = true;
	m_erodeFirst = true;
	m_kErode = 6;
	m_kDilate = 12;
	m_checkArea = true;
	m_minArea = 1;
	m_checkPerimeter = false;
	m_minPerimeter = 1;
	m_checkKForm = false;
	m_minKForm = 2.5;
}

Param::~Param()
{

}

void Param::setMaskUsed(bool value)
{
	m_maskUsed = value;
}

bool Param::isMaskUsed() const
{
	return m_maskUsed;
}

void Param::setErodeFirst(bool value)
{
	m_erodeFirst = value;
}

bool Param::isErodeFirst() const
{
	return m_erodeFirst;
}

void Param::setKErode(int value)
{
	m_kErode = value;
}

int Param::getKErode() const
{
	return m_kErode;
}

void Param::setKDilate(int value)
{
	m_kDilate = value;
}

int Param::getKDilate() const
{
	return m_kDilate;
}

void Param::setCheckArea(bool value)
{
	m_checkArea = value;
}

bool Param::isCheckArea() const
{
	return m_checkArea;
}

void Param::setMinArea(int value)
{
	m_minArea = value;
}

int Param::getMinArea() const
{
	return m_minArea;
}

void Param::setCheckPerimeter(bool value)
{
	m_checkPerimeter = value;
}

bool Param::isCheckPerimeter() const
{
	return m_checkPerimeter;
}

void Param::setMinPerimeter(int value)
{
	m_minPerimeter = value;
}

int Param::getMinPerimeter() const
{
	return m_minPerimeter;
}

void Param::setCheckKForm(bool value)
{
	m_checkKForm = value;
}

bool Param::isCheckKForm() const
{
	return m_checkKForm;
}

void Param::setMinKForm(double value)
{
	m_minKForm = value;
}

double Param::getMinKForm() const
{
	return m_minKForm;
}

} // namespace ICEFINDOBJECTPROC_NS
