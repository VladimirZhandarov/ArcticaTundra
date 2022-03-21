#include "param.h"

namespace ICEAVGLUMPROC_NS
{

Param::Param(QObject* parent) : QObject(parent)
{
	m_maskUsed = true;
	m_avgAbsoluteLum = 0;
	m_avgAbsoluteRed = 0;
	m_avgAbsoluteGreen = 0;
	m_avgAbsoluteBlue = 0;

	m_dontUseBlack = false;
	m_dontUseBlackValue = 0;
	m_dontUseWhite = false;
	m_dontUseWhiteValue = 765;

	m_baseImageRectX1 = 0;
	m_baseImageRectY1 = 0;
	m_baseImageRectX2 = -1;
	m_baseImageRectY2 = -1;
	m_useRectForBase = false;

	m_avgAbsoluteAutoDeltaLum = 0.0;
	m_avgAbsoluteAutoDeltaRed = 0.0;
	m_avgAbsoluteAutoDeltaGreen = 0.0;
	m_avgAbsoluteAutoDeltaBlue = 0.0;

	m_channelType = Y_TYPE;
	m_lum = 1.0;
	m_kMinLum = 0.9;
	m_kMaxLum = 1.1;
	m_kLum = 1.0;
	m_kLumUsed = false;
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

void Param::setAvgAbsoluteLum(int value)
{
	m_avgAbsoluteLum = value;
}

int Param::getAvgAbsoluteLum() const
{
	return m_avgAbsoluteLum;
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

void Param::setBaseImageRectX1(int value)
{
	m_baseImageRectX1 = value;
}

int Param::getBaseImageRectX1() const
{
	return m_baseImageRectX1;
}

void Param::setBaseImageRectY1(int value)
{
	m_baseImageRectY1 = value;
}

int Param::getBaseImageRectY1() const
{
	return m_baseImageRectY1;
}

void Param::setBaseImageRectX2(int value)
{
	m_baseImageRectX2 = value;
}

int Param::getBaseImageRectX2() const
{
	return m_baseImageRectX2;
}

void Param::setBaseImageRectY2(int value)
{
	m_baseImageRectY2 = value;
}

int Param::getBaseImageRectY2() const
{
	return m_baseImageRectY2;
}

void Param::setBaseImageRect(const QRect& value)
{
	m_baseImageRectX1 = value.topLeft().x();
	m_baseImageRectY1 = value.topLeft().y();
	m_baseImageRectX2 = value.bottomRight().x();
	m_baseImageRectY2 = value.bottomRight().y();
}

QRect Param::getBaseImageRect() const
{
	return QRect(QPoint(m_baseImageRectX1, m_baseImageRectY1),
		QPoint(m_baseImageRectX2, m_baseImageRectY2));
}

QString Param::getBaseSizeStringValue() const
{
	QRect rect = getBaseImageRect().normalized();
	return QString::number(rect.width()) + " x " + QString::number(rect.height());
}

void Param::setUseRectForBase(bool value)
{
	m_useRectForBase = value;
}

bool Param::isUseRectForBase() const
{
	return m_useRectForBase;
}

void Param::setAvgAbsoluteAutoDeltaLum(double value)
{
	m_avgAbsoluteAutoDeltaLum = value;
}

double Param::getAvgAbsoluteAutoDeltaLum() const
{
	return m_avgAbsoluteAutoDeltaLum;
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

void Param::setChannelType(YRGBChannelType value)
{
	m_channelType = value;
}

Param::YRGBChannelType Param::getChannelType() const
{
	return m_channelType;
}

void Param::setLum(double value)
{
	m_lum = value;
}

double Param::getLum() const
{
	return m_lum;
}

void Param::setKMinLum(double value)
{
	m_kMinLum = value;
}

double Param::getKMinLum() const
{
	return m_kMinLum;
}

void Param::setKMaxLum(double value)
{
	m_kMaxLum = value;
}

double Param::getKMaxLum() const
{
	return m_kMaxLum;
}

void Param::setKLum(double value)
{
	m_kLum = value;
}

double Param::getKLum() const
{
	return m_kLum;
}

void Param::setKLumUsed(bool value)
{
	m_kLumUsed = value;
}

bool Param::isKLumUsed() const
{
	return m_kLumUsed;
}

} // namespace ICEAVGLUMPROC_NS
