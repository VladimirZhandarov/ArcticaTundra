#ifndef ICEAVGLUMPROC_TOPITEMWIDGET_H
#define ICEAVGLUMPROC_TOPITEMWIDGET_H

#include "iceavglumprocdef.h"

#include <QtGui/QWidget>

#include "ui_topitemwidget.h"

#include "param.h"

#include <iceimage/imagebuilder.h>

namespace ICEPROC_NS
{
	class ImageProcInterface;
	class ImageWindowInterface;
}

namespace ICEAVGLUMPROC_NS
{

class ICEAVGLUMPROC_EXPORT TopItemWidget : public QWidget
{
	Q_OBJECT
public:
	TopItemWidget(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~TopItemWidget();

	void init(const ICEPROC_NS::ImageProcInterface* proc,
		ICEPROC_NS::ImageWindowInterface* imageWindowInterface);
	void clear();
	void setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb = false);
	QSharedPointer<QObject> getParamObject(int index) const;

	void setFilterParamId(unsigned int filterParamId);

	const QColor& getRedColor() const;
	const QColor& getGreenColor() const;
	const QColor& getBlueColor() const;
	void setImageBaseValue();
	bool prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder =
		QSharedPointer<ICEIMAGE_NS::ImageBuilder>());
	bool isCanUsed() const;

protected slots:
	void baseCalcPB_clicked();
	void baseCancelPB_clicked();
	void rb_toggled(bool checked);
	void dontUseBlackChB_stateChanged(int state);
	void dontUseWhiteChB_stateChanged(int state);

signals:
	void imageBaseValueChanged(int imageBaseValue);
	void maskPBEnabled(bool enabled);
	void channelTypeChanged(int channelType);
	void baseChanged();
	void paramFilterBaseChanged(QSharedPointer<QObject> paramFilterBase);

private:
	bool baseCalc(int maskType, const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder =
		QSharedPointer<ICEIMAGE_NS::ImageBuilder>());

	Ui::TopItemWidget ui;	///< Ui класс

	const ICEPROC_NS::ImageProcInterface* m_proc;
	ICEPROC_NS::ImageWindowInterface* m_imageWindowInterface;
	QColor m_redColor;
	QColor m_greenColor;
	QColor m_blueColor;
	Param::YRGBChannelType m_channelType;
	bool m_baseImageCalced;
	QSharedPointer<QObject> m_paramFilterBase;
	QSharedPointer<QObject> m_paramImageBase;
	unsigned int m_filterParamId;
};

} // namespace ICEAVGLUMPROC_NS

#endif // ICEAVGLUMPROC_TOPITEMWIDGET_H
