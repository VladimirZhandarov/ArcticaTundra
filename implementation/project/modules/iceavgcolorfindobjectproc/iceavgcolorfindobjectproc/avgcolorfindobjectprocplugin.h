#ifndef AVGCOLORFINDOBJECTPROCPLUGIN_H
#define AVGCOLORFINDOBJECTPROCPLUGIN_H

#include "iceavgcolorfindobjectprocdef.h"

#include <QtCore/QObject>
#include <iceproc/imageprocinterface.h>

namespace ICEAVGCOLORFINDOBJECTPROC_NS
{

class ICEAVGCOLORFINDOBJECTPROC_EXPORT AvgColorFindObjectProcPlugin : public QObject,
	ICEPROC_NS::ImageProcInterface
{
	Q_OBJECT
	Q_INTERFACES(ICEPROC_NS::ImageProcInterface)

public:
	AvgColorFindObjectProcPlugin();
	virtual ~AvgColorFindObjectProcPlugin();

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
	virtual ICEPROC_NS::EditWidget* createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const;

private:
	QList<QSharedPointer<ICEPROC_NS::ImageProcInterface> > m_subPluginList;
};

} // namespace ICEAVGCOLORFINDOBJECTPROC_NS

#endif // AVGCOLORFINDOBJECTPROCPLUGIN_H
