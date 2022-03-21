#ifndef IMAGEPROCINTERFACEQOBJECT_H
#define IMAGEPROCINTERFACEQOBJECT_H

#include "iceprocdef.h"

#include <QtCore/QObject>

namespace ICEPROC_NS
{

class ICEPROC_EXPORT ImageProcInterfaceQObject : public QObject
{
	Q_OBJECT
public:
	/**
	* Конструктор
	*/
	ImageProcInterfaceQObject(QObject* parent = 0);

	/**
	* Деструктор
	*/
	virtual ~ImageProcInterfaceQObject();

	void emitMaskProgressRangeChanged(int minimum, int maximum);
	void emitMaskProgressValueChanged(int progressValue);
	void emitMaskProgressTextChanged(const QString& progressText);

	void emitFilterCalculated(int filterNum, const QString& filterType);

signals:
	void maskProgressRangeChanged(int minimum, int maximum);
	void maskProgressValueChanged(int progressValue);
	void maskProgressTextChanged(const QString& progressText);

	void filterCalculated(int filterNum, const QString& filterType);
};

} // namespace ICEPROC_NS

#endif // IMAGEPROCINTERFACEQOBJECT_H
