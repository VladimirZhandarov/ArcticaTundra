#ifndef AVGCOLORPROCPLUGIN_H
#define AVGCOLORPROCPLUGIN_H

#include "iceavgcolorprocdef.h"

#include <QtCore/QObject>
#include <iceproc/imageprocinterface.h>

namespace ICEAVGCOLORPROC_NS
{

class ICEAVGCOLORPROC_EXPORT AvgColorProcPlugin : public QObject, ICEPROC_NS::ImageProcInterface
{
	Q_OBJECT
	Q_INTERFACES(ICEPROC_NS::ImageProcInterface)

public:
	/**
	* Конструктор
	*/
	AvgColorProcPlugin();

	/**
	* Деструктор
	*/
	virtual ~AvgColorProcPlugin();

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::createParamObject()
	*/
	virtual QSharedPointer<QObject> createParamObject(int index) const;

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::getShortName()
	*/
	virtual QString getShortName() const;

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::calcMaskVector()
	*/
	virtual QVector<int> calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
		const QVector<int>& indexVector = QVector<int>(), double divK = 1.0,
		int unit = 0);

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::calcParamObject()
	*/
	virtual QSharedPointer<QObject> calcParamObject(int index, QSharedPointer<QObject> param,
		const QImage& image, const QRect& rect) const;

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::getViewName()
	*/
	virtual QString getViewName(bool severalItems = false) const;

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::getViewIcon()
	*/
	virtual QIcon getViewIcon() const;

protected:
	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::createEditWidget()
	*/
	virtual ICEPROC_NS::EditWidget* createEditWidget(
		ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const;
};

} // namespace ICEAVGCOLORPROC_NS

#endif // AVGCOLORPROCPLUGIN_H
