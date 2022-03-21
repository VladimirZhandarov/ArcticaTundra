#ifndef AVGLUMPROCPLUGIN_H
#define AVGLUMPROCPLUGIN_H

#include "iceavglumprocdef.h"

#include <QtCore/QObject>
#include <iceproc/imageprocinterface.h>

namespace ICEAVGLUMPROC_NS
{

class ICEAVGLUMPROC_EXPORT AvgLumProcPlugin : public QObject, ICEPROC_NS::ImageProcInterface
{
	Q_OBJECT
	Q_INTERFACES(ICEPROC_NS::ImageProcInterface)

public:
	AvgLumProcPlugin();
	~AvgLumProcPlugin();

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

	virtual bool prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);

protected:
		/**
	* @copydoc ICEPROC_NS::ImageProcInterface::createEditWidget()
	*/
	virtual ICEPROC_NS::EditWidget* createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const;
};

} // namespace ICEAVGLUMPROC_NS

#endif // AVGLUMPROCPLUGIN_H
