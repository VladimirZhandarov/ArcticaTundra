#ifndef IMAGEPROCINTERFACE_H
#define IMAGEPROCINTERFACE_H

#include "iceprocdef.h"

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QSharedPointer>

#include <boost/tr1/memory.hpp>

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEODB_NS
{
	class FilterParam;
	class MaskParam;
	class MaskView;
	class LineView;
}

namespace ICEPROC_NS
{
	class EditWidget;
	class DefaultValuesWidget;
	class ImageWindowInterface;
	class ImageProcInterfaceQObject;

/**
* @class ImageProcInterface
* Класс plugin алгоритма для обработки изображений
*/
class ICEPROC_EXPORT ImageProcInterface
{
public:
	/**
	* Конструктор
	*/
	ImageProcInterface();

	/**
	* Деструктор
	*/
	virtual ~ImageProcInterface();

	virtual void init();

	/**
	* Установить имя dll-файла
	* @param dllName имя файла
	*/
	void setDllName(const QString& dllName);

	/**
	* Получить имя dll-файла
	* @return dllName имя файла
	*/
	const QString& getDllName() const;

	/**
	* Получить QWidget редактирования параметров алгоритма обработки изображения
	* @return QWidget редактирования параметров алгоритма обработки изображения
	*/
	EditWidget* getEditWidget(ImageWindowInterface* imageWindowInterface);

	virtual QSharedPointer<QObject> createParamObject(int index) const = 0;
	virtual QString getShortName() const = 0;

	virtual int calcTotalSteps(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);
	/**
	* Посчитать попадание точек под установленные параметры
	* @param imageBuilder
	* @param filterParam
	* @param indexVector
	*/
	virtual QVector<int> calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, const QVector<int>& indexVector = QVector<int>(),
		double divK = 1.0, int unit = 0) = 0;

	bool prepAndCalcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, double divK = 1.0,
		int unit = 0);

	virtual QSharedPointer<QObject> calcParamObject(int index, QSharedPointer<QObject> param,
		const QImage& image, const QRect& rect) const;

	/**
	* Получить имя плагина (алгоритма)
	* @param severalItems
	* @return имя плагина (алгоритма)
	*/
	virtual QString getViewName(bool severalItems = false) const = 0;

	/**
	* Получить иконку
	* @return иконка
	*/
	virtual QIcon getViewIcon() const = 0;

	static QString getCudaDeviceProperties(int devId = -1);

	QSharedPointer<QObject> fromJSonString(int index, const QString& jsonString) const;

	QString toJSonString(const QSharedPointer<QObject>& param) const;

	virtual bool prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);

	ImageProcInterfaceQObject* getQObject() const;

protected:
	/**
	* Конструирование QWidget редактирования параметров алгоритма обработки изображения
	* @return QWidget редактирования параметров алгоритма обработки изображения
	*/
	virtual EditWidget* createEditWidget(ImageWindowInterface* imageWindowInterface) const;

	/**
	* Заполнить параметры окна параметров алгоритма обработки изображения
	* @param widget окно для радактирования параметров алгоритма обработки изображения
	*/
	void fillEditWidget(EditWidget* widget) const;

	QVector<QSharedPointer<QObject> > getParamVector(
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const;
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > getMaskViewVector(
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const;
	QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > getLineViewVector(
		const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const;
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> createMaskParam(const QSharedPointer<QObject>& param,
		const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView,
		const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);

	void emitMaskProgressRangeChanged(int minimum, int maximum);
	void emitMaskProgressValueChanged(int progressValue);
	void emitMaskProgressTextChanged(const QString& progressText);

	void emitFilterCalculated(int filterNum, const QString& filterType);

private:
	QString m_dllName;	///< Имя dll-файла
	ImageProcInterfaceQObject* m_qObject;
};

} // namespace ICEPROC_NS

Q_DECLARE_INTERFACE(ICEPROC_NS::ImageProcInterface, "com.dvg.Plugin.ImageProcInterface/1.0");

#endif // IMAGEPROCINTERFACE_H
