#include "iceunitratioqwttransform.h"

#include <QtCore/QObject>

namespace ICEQWT_NS
{

static const int MMinSM = 10;
static const int MMinM = 1000;
static const int MMinKM = 1000000;

static const int SMinM = 100;
static const int SMinKM = 100000;

static const int MinKM = 1000;

/*
  Концепция: есть две краиние позиции - это пиксели и 
  метрическая система, в которую входят метры, километры
  и т.д. который между собой связаны математически, поэтому
  они и относятся к одной позиции. Нюансы возникают при 
  переходе из одной позиции в другую. При переходе из метрической
  системы в пиксели важно сохранить ту единицу измерения, которая
  использовалась в метрической системе и при этом не изменить 
  коэффициент. Т.е. состояние метрической системы остается нетронутым 
  и восстанавливается когда происходит переход из пикселей в 
  метрическую систему
*/

class IceUnitRatioQwtTransform::PrivateData
{
public:
	PrivateData() : unit(ICEIMAGE_NS::NONE), metricUnit(ICEIMAGE_NS::M), ratio(1.0)
	{

	}

	ICEIMAGE_NS::UNIT unit;
	ICEIMAGE_NS::UNIT metricUnit;	 // Единица измерения метрической системе
									// Кол-во пикселей в установленной единице измерения (коэффициент)
	double ratio; // Всегда соотвествует метрической системе
};

IceUnitRatioQwtTransform::IceUnitRatioQwtTransform() : QwtTransform()
{
	d_data = new PrivateData;
}

IceUnitRatioQwtTransform::~IceUnitRatioQwtTransform()
{
	delete d_data;
}

QString IceUnitRatioQwtTransform::unitName(ICEIMAGE_NS::UNIT unit)
{
	switch(unit)
	{
		case ICEIMAGE_NS::MM:
		{
			return QObject::tr("Милиметры");
		}
		case ICEIMAGE_NS::SM:
		{
			return QObject::tr("Сантиметры");
		}
		case ICEIMAGE_NS::M:
		{
			return QObject::tr("Метры");
		}
		case ICEIMAGE_NS::KM:
		{
			return QObject::tr("Километры");
		}
		case ICEIMAGE_NS::NONE:
		{
			return QObject::tr("Пиксели");
		}
	}
	return "";
}

ICEIMAGE_NS::UNIT IceUnitRatioQwtTransform::unit() const
{
	return d_data->unit;
}

void IceUnitRatioQwtTransform::unit(QString& str) const
{
	str = unitName(unit());
}

void IceUnitRatioQwtTransform::setUnit(ICEIMAGE_NS::UNIT unit)
{
	if(IceUnitRatioQwtTransform::unit() == unit)
	{
		return;
	}
	switch(d_data->unit)
	{
		case ICEIMAGE_NS::MM:
		{
			switch(unit)
			{
				case ICEIMAGE_NS::SM:
				{
					d_data->ratio *= MMinSM;
					break;
				}
				case ICEIMAGE_NS::M:
				{
					d_data->ratio *= MMinM;
					break;
				}
				case ICEIMAGE_NS::KM:
				{
					d_data->ratio *= MMinKM;
					break;
				}
				// Переход из метрической системы в пиксели (1)
				case ICEIMAGE_NS::NONE:
				default:
				{
					d_data->metricUnit = d_data->unit;
					break;
				}
			}
			break;
		}
		case ICEIMAGE_NS::SM:
		{
			switch(unit)
			{
				case ICEIMAGE_NS::MM:
				{
					d_data->ratio /= MMinSM;
					break;
				}
				case ICEIMAGE_NS::M:
				{
					d_data->ratio  *= SMinM;
					break;
				}
				case ICEIMAGE_NS::KM:
				{
					d_data->ratio *= SMinKM;
					break;
				}
				// см. (1)
				case ICEIMAGE_NS::NONE:
				default:
				{
					d_data->metricUnit = d_data->unit;
					break;
				}
			}
			break;
		}
		case ICEIMAGE_NS::M:
		{
			switch(unit)
			{
				case ICEIMAGE_NS::MM:
				{
					d_data->ratio /= MMinM;
					break;
				}
				case ICEIMAGE_NS::SM:
				{
					d_data->ratio /= SMinM;
					break;
				}
				case ICEIMAGE_NS::KM:
				{
					d_data->ratio *= MinKM;
					break;
				}
				// см. (1)
				case ICEIMAGE_NS::NONE:
				default:
				{
					d_data->metricUnit = d_data->unit;
					break;
				}
			}
			break;
		}
		case ICEIMAGE_NS::KM:
		{
			switch(unit)
			{
				case ICEIMAGE_NS::MM:
				{
					d_data->ratio /= MMinKM;
					break;
				}
				case ICEIMAGE_NS::SM:
				{
					d_data->ratio /= SMinKM;
					break;
				}
				case ICEIMAGE_NS::M:
				{
					d_data->ratio  /= MinKM;
					break;
				}
				// см. (1)
				case ICEIMAGE_NS::NONE:
				default:
				{
					d_data->metricUnit = d_data->unit;
					break;
				}
			}
			break;
		}
		case ICEIMAGE_NS::NONE:
		default:
		{
			// Переход из пикселей в метрическую систему
			d_data->unit = d_data->metricUnit;	// Восстанавливаем состояние метрической системы
			setUnit(unit);						// и запускаем еще раз
			break;//d_data->ratio = 1;
		}
	}
	d_data->unit = unit;
}

double IceUnitRatioQwtTransform::ratio() const
{
	if(d_data->unit == ICEIMAGE_NS::NONE)
	{
		return 1; // Чтобы не менять коэффициент метрической системы
	}
	return d_data->ratio;
}

void IceUnitRatioQwtTransform::setRatio(double ratio)
{
	if(d_data->unit != ICEIMAGE_NS::NONE)
	{
		d_data->ratio = ratio;
	}
}

void IceUnitRatioQwtTransform::setRatio(double ratio, ICEIMAGE_NS::UNIT unit)
{
	if(unit != ICEIMAGE_NS::NONE)
	{
		d_data->metricUnit = unit;
		d_data->ratio = ratio;
	}
	d_data->unit = unit;
}

ICEIMAGE_NS::UNIT IceUnitRatioQwtTransform::metricUnit() const
{
	return d_data->metricUnit;
}
 
double IceUnitRatioQwtTransform::metricRatio() const
{
	return d_data->ratio;
}

double IceUnitRatioQwtTransform::transform(double value) const
{
	return value * ratio();
}

double IceUnitRatioQwtTransform::invTransform(double value) const
{
	return value / ratio();
}

QPointF IceUnitRatioQwtTransform::transform(const QPointF& point) const
{
	QPointF result(point);
	result.setX(transform(result.x()));
	result.setY(transform(result.y()));
	return result;
}

QPointF IceUnitRatioQwtTransform::invTransform(const QPointF& point) const
{
	QPointF result(point);
	result.setX(invTransform(result.x()));
	result.setY(invTransform(result.y()));
	return result;
}

QRectF IceUnitRatioQwtTransform::transform(const QRectF& rect) const
{
	QRectF result;
	result.setTopLeft(transform(rect.topLeft()));
	result.setBottomRight(transform(rect.bottomRight()));
	return result;
}

QRectF IceUnitRatioQwtTransform::invTransform(const QRectF& rect) const
{
	QRectF result;
	result.setTopLeft(invTransform(rect.topLeft()));
	result.setBottomRight(invTransform(rect.bottomRight()));
	return result;
}

QwtTransform *IceUnitRatioQwtTransform::copy() const
{
	IceUnitRatioQwtTransform* transform = new IceUnitRatioQwtTransform();
	transform->d_data->unit = d_data->unit;
	transform->d_data->ratio = d_data->ratio;
	transform->d_data->metricUnit = d_data->metricUnit;
	return transform;
}

} // namespace ICEQWT_NS

