#ifndef FINDOBJECTPROCPLUGIN_H
#define FINDOBJECTPROCPLUGIN_H

#include "icefindobjectprocdef.h"

#include <QtCore/QObject>
#include <iceproc/imageprocinterface.h>

namespace ICEFINDOBJECTPROC_NS
{

class ICEFINDOBJECTPROC_EXPORT FindObjectProcPlugin : public QObject, ICEPROC_NS::ImageProcInterface
{
	Q_OBJECT
	Q_INTERFACES(ICEPROC_NS::ImageProcInterface)

public:
	FindObjectProcPlugin();
	virtual ~FindObjectProcPlugin();

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
	* @copydoc ICEPROC_NS::ImageProcInterface::getViewName()
	*/
	virtual QString getViewName(bool severalItems = false) const;

	/**
	* @copydoc ICEPROC_NS::ImageProcInterface::getViewIcon()
	*/
	virtual QIcon getViewIcon() const;

protected:
	virtual ICEPROC_NS::EditWidget* createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const;
};

} // namespace ICEFINDOBJECTPROC_NS

#endif // FINDOBJECTPROCPLUGIN_H
