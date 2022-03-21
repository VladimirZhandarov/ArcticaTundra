#ifndef PARAM_H
#define PARAM_H

#include <QtCore/QObject>

#include <QtCore/QVariant>
#include <QtCore/QRect>

namespace ICEAVGLUMPROC_NS
{

class Param : public QObject
{
	Q_OBJECT
public:
	enum MaskType
	{
		FILTER_BASE_MASK_TYPE = 0,
		IMAGE_BASE_MASK_TYPE,
		MASK_TYPE
	};

	enum YRGBChannelType
	{
		Y_TYPE = 0,
		R_TYPE,
		G_TYPE,
		B_TYPE
	};

private:
	Q_PROPERTY(bool m_maskUsed READ isMaskUsed WRITE setMaskUsed)

	Q_PROPERTY(int m_avgAbsoluteLum READ getAvgAbsoluteLum WRITE setAvgAbsoluteLum)
	Q_PROPERTY(int m_avgAbsoluteRed READ getAvgAbsoluteRed WRITE setAvgAbsoluteRed)
	Q_PROPERTY(int m_avgAbsoluteGreen READ getAvgAbsoluteGreen WRITE setAvgAbsoluteGreen)
	Q_PROPERTY(int m_avgAbsoluteBlue READ getAvgAbsoluteBlue WRITE setAvgAbsoluteBlue)

	Q_PROPERTY(bool m_dontUseBlack READ isDontUseBlack WRITE setDontUseBlack)
	Q_PROPERTY(int m_dontUseBlackValue READ getDontUseBlackValue WRITE setDontUseBlackValue)
	Q_PROPERTY(bool m_dontUseWhite READ isDontUseWhite WRITE setDontUseWhite)
	Q_PROPERTY(int m_dontUseWhiteValue READ getDontUseWhiteValue WRITE setDontUseWhiteValue)

	Q_PROPERTY(int m_baseImageRectX1 READ getBaseImageRectX1 WRITE setBaseImageRectX1)
	Q_PROPERTY(int m_baseImageRectY1 READ getBaseImageRectY1 WRITE setBaseImageRectY1)
	Q_PROPERTY(int m_baseImageRectX2 READ getBaseImageRectX2 WRITE setBaseImageRectX2)
	Q_PROPERTY(int m_baseImageRectY2 READ getBaseImageRectY2 WRITE setBaseImageRectY2)
	Q_PROPERTY(bool m_useRectForBase READ isUseRectForBase WRITE setUseRectForBase)

	Q_PROPERTY(double m_avgAbsoluteAutoDeltaLum READ getAvgAbsoluteAutoDeltaLum WRITE setAvgAbsoluteAutoDeltaLum)
	Q_PROPERTY(double m_avgAbsoluteAutoDeltaRed READ getAvgAbsoluteAutoDeltaRed WRITE setAvgAbsoluteAutoDeltaRed)
	Q_PROPERTY(double m_avgAbsoluteAutoDeltaGreen READ getAvgAbsoluteAutoDeltaGreen WRITE setAvgAbsoluteAutoDeltaGreen)
	Q_PROPERTY(double m_avgAbsoluteAutoDeltaBlue READ getAvgAbsoluteAutoDeltaBlue WRITE setAvgAbsoluteAutoDeltaBlue)

	Q_PROPERTY(YRGBChannelType m_channelType READ getChannelType WRITE setChannelType)
	Q_PROPERTY(double m_lum READ getLum WRITE setLum)
	Q_PROPERTY(double m_kMinLum READ getKMinLum WRITE setKMinLum)
	Q_PROPERTY(double m_kMaxLum READ getKMaxLum WRITE setKMaxLum)
	Q_PROPERTY(double m_kLum READ getKLum WRITE setKLum)
	Q_PROPERTY(bool m_kLumUsed READ isKLumUsed WRITE setKLumUsed)

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

	void setAvgAbsoluteLum(int value);
	int getAvgAbsoluteLum() const;

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

	void setBaseImageRectX1(int value);
	int getBaseImageRectX1() const;

	void setBaseImageRectY1(int value);
	int getBaseImageRectY1() const;
	
	void setBaseImageRectX2(int value);
	int getBaseImageRectX2() const;
	
	void setBaseImageRectY2(int value);
	int getBaseImageRectY2() const;
	
	void setBaseImageRect(const QRect& value);
	QRect getBaseImageRect() const;
	
	QString getBaseSizeStringValue() const;

	void setUseRectForBase(bool value);
	bool isUseRectForBase() const;

	void setAvgAbsoluteAutoDeltaLum(double value);
	double getAvgAbsoluteAutoDeltaLum() const;

	void setAvgAbsoluteAutoDeltaRed(double value);
	double getAvgAbsoluteAutoDeltaRed() const;

	void setAvgAbsoluteAutoDeltaGreen(double value);
	double getAvgAbsoluteAutoDeltaGreen() const;

	void setAvgAbsoluteAutoDeltaBlue(double value);
	double getAvgAbsoluteAutoDeltaBlue() const;

	void setChannelType(YRGBChannelType value);
	YRGBChannelType getChannelType() const;

	void setLum(double value);
	double getLum() const;

	void setKMinLum(double value);
	double getKMinLum() const;

	void setKMaxLum(double value);
	double getKMaxLum() const;

	void setKLum(double value);
	double getKLum() const;

	void setKLumUsed(bool value);
	bool isKLumUsed() const;

private:
	bool m_maskUsed;

	int m_avgAbsoluteLum;
	int m_avgAbsoluteRed;
	int m_avgAbsoluteGreen;
	int m_avgAbsoluteBlue;

	bool m_dontUseBlack;
	int m_dontUseBlackValue;
	bool m_dontUseWhite;
	int m_dontUseWhiteValue;

	int m_baseImageRectX1;
	int m_baseImageRectY1;
	int m_baseImageRectX2;
	int m_baseImageRectY2;
	bool m_useRectForBase;

	double m_avgAbsoluteAutoDeltaLum;
	double m_avgAbsoluteAutoDeltaRed;
	double m_avgAbsoluteAutoDeltaGreen;
	double m_avgAbsoluteAutoDeltaBlue;

	YRGBChannelType m_channelType;

	double m_lum;
	double m_kMinLum;
	double m_kMaxLum;
	double m_kLum;
	bool m_kLumUsed;
};

} // namespace ICEAVGLUMPROC_NS

#endif // PARAM_H
