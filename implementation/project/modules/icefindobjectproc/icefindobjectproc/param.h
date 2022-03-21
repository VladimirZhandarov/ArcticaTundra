#ifndef PARAM_H
#define PARAM_H

#include <QtCore/QObject>

#include <QtCore/QVariant>

namespace ICEFINDOBJECTPROC_NS
{

class Param : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool m_maskUsed READ isMaskUsed WRITE setMaskUsed)
	Q_PROPERTY(bool m_erodeFirst READ isErodeFirst WRITE setErodeFirst)
	Q_PROPERTY(int m_kErode READ getKErode WRITE setKErode)
	Q_PROPERTY(int m_kDilate READ getKDilate WRITE setKDilate)
	Q_PROPERTY(bool m_checkArea READ isCheckArea WRITE setCheckArea)
	Q_PROPERTY(int m_minArea READ getMinArea WRITE setMinArea)
	Q_PROPERTY(bool m_checkPerimeter READ isCheckPerimeter WRITE setCheckPerimeter)
	Q_PROPERTY(int m_minPerimeter READ getMinPerimeter WRITE setMinPerimeter)
	Q_PROPERTY(bool m_checkKForm READ isCheckKForm WRITE setCheckKForm)
	Q_PROPERTY(double m_minKForm READ getMinKForm WRITE setMinKForm)

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

	void setErodeFirst(bool value);
	bool isErodeFirst() const;

	void setKErode(int value);
	int getKErode() const;

	void setKDilate(int value);
	int getKDilate() const;

	void setCheckArea(bool value);
	bool isCheckArea() const;

	void setMinArea(int value);
	int getMinArea() const;

	void setCheckPerimeter(bool value);
	bool isCheckPerimeter() const;

	void setMinPerimeter(int value);
	int getMinPerimeter() const;

	void setCheckKForm(bool value);
	bool isCheckKForm() const;

	void setMinKForm(double value);
	double getMinKForm() const;

private:
	bool m_maskUsed;
	bool m_erodeFirst;
	int m_kErode;
	int m_kDilate;
	bool m_checkArea;
	int m_minArea;
	bool m_checkPerimeter;
	int m_minPerimeter;
	bool m_checkKForm;
	double m_minKForm;
};

} // namespace ICEFINDOBJECTPROC_NS

#endif // PARAM_H
