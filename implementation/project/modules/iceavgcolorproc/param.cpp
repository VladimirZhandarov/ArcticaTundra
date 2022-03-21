#include "param.h"

namespace ICEAVGCOLORPROC_NS
{

Param::Param(QObject* parent) : QObject(parent)
{
	m_maskUsed = true;
	m_avgRelativeRed = 1.0 / 3.0;
	m_avgRelativeGreen = 1.0 / 3.0;
	m_avgRelativeBlue = 1.0 / 3.0;

	m_avgAbsoluteRed = 0;
	m_avgAbsoluteGreen = 0;
	m_avgAbsoluteBlue = 0;

	m_dontUseBlack = false;
	m_dontUseBlackValue = 0;
	m_dontUseWhite = false;
	m_dontUseWhiteValue = 765;

	m_deltaMinusRelativeRed = 0.03;
	m_deltaMinusRelativeGreen = 0.03;
	m_deltaMinusRelativeBlue = 0.03;

	m_deltaPlusRelativeRed = 0.03;
	m_deltaPlusRelativeGreen = 0.03;
	m_deltaPlusRelativeBlue = 0.03;

	m_avgAbsoluteAutoDeltaRed = 0.0;
	m_avgAbsoluteAutoDeltaGreen = 0.0;
	m_avgAbsoluteAutoDeltaBlue = 0.0;

	m_kRgb = 1.0;
	m_kRgbUsed = false;
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

void Param::setAvgRelativeRed(double value)
{
	m_avgRelativeRed = value;
}

double Param::getAvgRelativeRed() const
{
	return m_avgRelativeRed;
}

void Param::setAvgRelativeGreen(double value)
{
	m_avgRelativeGreen = value;
}

double Param::getAvgRelativeGreen() const
{
	return m_avgRelativeGreen;
}

void Param::setAvgRelativeBlue(double value)
{
	m_avgRelativeBlue = value;
}

double Param::getAvgRelativeBlue() const
{
	return m_avgRelativeBlue;
}

void Param::setAvgAbsoluteRed(int value)
{
	m_avgAbsoluteRed = value;
}

int Param::getAvgAbsoluteRed() const
{
	return m_avgAbsoluteRed;
}

void Param::setAvgAbsoluteGreen(int value)
{
	m_avgAbsoluteGreen = value;
}

int Param::getAvgAbsoluteGreen() const
{
	return m_avgAbsoluteGreen;
}

void Param::setAvgAbsoluteBlue(int value)
{
	m_avgAbsoluteBlue = value;
}

int Param::getAvgAbsoluteBlue() const
{
	return m_avgAbsoluteBlue;
}

void Param::setDontUseBlack(bool value)
{
	m_dontUseBlack = value;
}

bool Param::isDontUseBlack() const
{
	return m_dontUseBlack;
}

void Param::setDontUseBlackValue(int value)
{
	m_dontUseBlackValue = value;
}

int Param::getDontUseBlackValue() const
{
	return m_dontUseBlackValue;
}

void Param::setDontUseWhite(bool value)
{
	m_dontUseWhite = value;
}

bool Param::isDontUseWhite() const
{
	return m_dontUseWhite;
}

void Param::setDontUseWhiteValue(int value)
{
	m_dontUseWhiteValue = value;
}

int Param::getDontUseWhiteValue() const
{
	return m_dontUseWhiteValue;
}

void Param::setDeltaMinusRelativeRed(double value)
{
	m_deltaMinusRelativeRed = value;
}

double Param::getDeltaMinusRelativeRed() const
{
	return m_deltaMinusRelativeRed;
}

void Param::setDeltaMinusRelativeGreen(double value)
{
	m_deltaMinusRelativeGreen = value;
}

double Param::getDeltaMinusRelativeGreen() const
{
	return m_deltaMinusRelativeGreen;
}

void Param::setDeltaMinusRelativeBlue(double value)
{
	m_deltaMinusRelativeBlue = value;
}

double Param::getDeltaMinusRelativeBlue() const
{
	return m_deltaMinusRelativeBlue;
}

void Param::setDeltaPlusRelativeRed(double value)
{
	m_deltaPlusRelativeRed = value;
}

double Param::getDeltaPlusRelativeRed() const
{
	return m_deltaPlusRelativeRed;
}

void Param::setDeltaPlusRelativeGreen(double value)
{
	m_deltaPlusRelativeGreen = value;
}

double Param::getDeltaPlusRelativeGreen() const
{
	return m_deltaPlusRelativeGreen;
}

void Param::setDeltaPlusRelativeBlue(double value)
{
	m_deltaPlusRelativeBlue = value;
}

double Param::getDeltaPlusRelativeBlue() const
{
	return m_deltaPlusRelativeBlue;
}

void Param::setAvgAbsoluteAutoDeltaRed(double value)
{
	m_avgAbsoluteAutoDeltaRed = value;
}

double Param::getAvgAbsoluteAutoDeltaRed() const
{
	return m_avgAbsoluteAutoDeltaRed;
}

void Param::setAvgAbsoluteAutoDeltaGreen(double value)
{
	m_avgAbsoluteAutoDeltaGreen = value;
}

double Param::getAvgAbsoluteAutoDeltaGreen() const
{
	return m_avgAbsoluteAutoDeltaGreen;
}

void Param::setAvgAbsoluteAutoDeltaBlue(double value)
{
	m_avgAbsoluteAutoDeltaBlue = value;
}

double Param::getAvgAbsoluteAutoDeltaBlue() const
{
	return m_avgAbsoluteAutoDeltaBlue;
}

void Param::setKRgb(double value)
{
	m_kRgb = value;
}

double Param::getKRgb() const
{
	return m_kRgb;
}

void Param::setKRgbUsed(bool value)
{
	m_kRgbUsed = value;
}

bool Param::isKRgbUsed() const
{
	return m_kRgbUsed;
}

} // namespace ICEAVGCOLORPROC_NS
