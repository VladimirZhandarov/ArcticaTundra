#ifndef PARAM_H
#define PARAM_H

#include <QtCore/QObject>

#include <QtCore/QVariant>

namespace ICEAVGCOLORPROC_NS
{

class Param : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool m_maskUsed READ isMaskUsed WRITE setMaskUsed)

	Q_PROPERTY(double m_avgRelativeRed READ getAvgRelativeRed WRITE setAvgRelativeRed)
	Q_PROPERTY(double m_avgRelativeGreen READ getAvgRelativeGreen WRITE setAvgRelativeGreen)
	Q_PROPERTY(double m_avgRelativeBlue READ getAvgRelativeBlue WRITE setAvgRelativeBlue)

	Q_PROPERTY(int m_avgAbsoluteRed READ getAvgAbsoluteRed WRITE setAvgAbsoluteRed)
	Q_PROPERTY(int m_avgAbsoluteGreen READ getAvgAbsoluteGreen WRITE setAvgAbsoluteGreen)
	Q_PROPERTY(int m_avgAbsoluteBlue READ getAvgAbsoluteBlue WRITE setAvgAbsoluteBlue)

	Q_PROPERTY(bool m_dontUseBlack READ isDontUseBlack WRITE setDontUseBlack)
	Q_PROPERTY(int m_dontUseBlackValue READ getDontUseBlackValue WRITE setDontUseBlackValue)
	Q_PROPERTY(bool m_dontUseWhite READ isDontUseWhite WRITE setDontUseWhite)
	Q_PROPERTY(int m_dontUseWhiteValue READ getDontUseWhiteValue WRITE setDontUseWhiteValue)

	Q_PROPERTY(double m_deltaMinusRelativeRed READ getDeltaMinusRelativeRed WRITE setDeltaMinusRelativeRed)
	Q_PROPERTY(double m_deltaMinusRelativeGreen READ getDeltaMinusRelativeGreen WRITE setDeltaMinusRelativeGreen)
	Q_PROPERTY(double m_deltaMinusRelativeBlue READ getDeltaMinusRelativeBlue WRITE setDeltaMinusRelativeBlue)

	Q_PROPERTY(double m_deltaPlusRelativeRed READ getDeltaPlusRelativeRed WRITE setDeltaPlusRelativeRed)
	Q_PROPERTY(double m_deltaPlusRelativeGreen READ getDeltaPlusRelativeGreen WRITE setDeltaPlusRelativeGreen)
	Q_PROPERTY(double m_deltaPlusRelativeBlue READ getDeltaPlusRelativeBlue WRITE setDeltaPlusRelativeBlue)

	Q_PROPERTY(double m_avgAbsoluteAutoDeltaRed READ getAvgAbsoluteAutoDeltaRed WRITE setAvgAbsoluteAutoDeltaRed)
	Q_PROPERTY(double m_avgAbsoluteAutoDeltaGreen READ getAvgAbsoluteAutoDeltaGreen WRITE setAvgAbsoluteAutoDeltaGreen)
	Q_PROPERTY(double m_avgAbsoluteAutoDeltaBlue READ getAvgAbsoluteAutoDeltaBlue WRITE setAvgAbsoluteAutoDeltaBlue)

	Q_PROPERTY(double m_kRgb READ getKRgb WRITE setKRgb)
	Q_PROPERTY(bool m_kRgbUsed READ isKRgbUsed WRITE setKRgbUsed)

public:
	/**
	* Конструктор
	* @param parent родительский объект
	*/
	Param(QObject* parent = 0);

	/**
	* Деструктор
	*/
	~Param();

	void setMaskUsed(bool value);
	bool isMaskUsed() const;

	void setAvgRelativeRed(double value);
	double getAvgRelativeRed() const;

	void setAvgRelativeGreen(double value);
	double getAvgRelativeGreen() const;

	void setAvgRelativeBlue(double value);
	double getAvgRelativeBlue() const;

	void setAvgAbsoluteRed(int value);
	int getAvgAbsoluteRed() const;

	void setAvgAbsoluteGreen(int value);
	int getAvgAbsoluteGreen() const;

	void setAvgAbsoluteBlue(int value);
	int getAvgAbsoluteBlue() const;

	void setDontUseBlack(bool value);
	bool isDontUseBlack() const;

	void setDontUseBlackValue(int value);
	int getDontUseBlackValue() const;

	void setDontUseWhite(bool value);
	bool isDontUseWhite() const;

	void setDontUseWhiteValue(int value);
	int getDontUseWhiteValue() const;

	void setDeltaMinusRelativeRed(double value);
	double getDeltaMinusRelativeRed() const;

	void setDeltaMinusRelativeGreen(double value);
	double getDeltaMinusRelativeGreen() const;

	void setDeltaMinusRelativeBlue(double value);
	double getDeltaMinusRelativeBlue() const;

	void setDeltaPlusRelativeRed(double value);
	double getDeltaPlusRelativeRed() const;

	void setDeltaPlusRelativeGreen(double value);
	double getDeltaPlusRelativeGreen() const;

	void setDeltaPlusRelativeBlue(double value);
	double getDeltaPlusRelativeBlue() const;

	void setAvgAbsoluteAutoDeltaRed(double value);
	double getAvgAbsoluteAutoDeltaRed() const;

	void setAvgAbsoluteAutoDeltaGreen(double value);
	double getAvgAbsoluteAutoDeltaGreen() const;

	void setAvgAbsoluteAutoDeltaBlue(double value);
	double getAvgAbsoluteAutoDeltaBlue() const;

	void setKRgb(double value);
	double getKRgb() const;

	void setKRgbUsed(bool value);
	bool isKRgbUsed() const;

private:
	bool m_maskUsed;

	double m_avgRelativeRed;
	double m_avgRelativeGreen;
	double m_avgRelativeBlue;

	int m_avgAbsoluteRed;
	int m_avgAbsoluteGreen;
	int m_avgAbsoluteBlue;

	bool m_dontUseBlack;
	int m_dontUseBlackValue;
	bool m_dontUseWhite;
	int m_dontUseWhiteValue;

	double m_deltaMinusRelativeRed;
	double m_deltaMinusRelativeGreen;
	double m_deltaMinusRelativeBlue;

	double m_deltaPlusRelativeRed;
	double m_deltaPlusRelativeGreen;
	double m_deltaPlusRelativeBlue;

	double m_avgAbsoluteAutoDeltaRed;
	double m_avgAbsoluteAutoDeltaGreen;
	double m_avgAbsoluteAutoDeltaBlue;

	double m_kRgb;
	bool m_kRgbUsed;
};

} // namespace ICEAVGCOLORPROC_NS

#endif // PARAM_H
