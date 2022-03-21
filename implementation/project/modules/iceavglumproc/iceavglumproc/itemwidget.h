#ifndef ICEAVGLUMPROC_ITEMWIDGET_H
#define ICEAVGLUMPROC_ITEMWIDGET_H

#include "iceavglumprocdef.h"

#include <QtGui/QWidget>

#include "ui_itemwidget.h"

#include "param.h"

namespace ICEPROC_NS
{
	class ImageProcInterface;
	class ImageWindowInterface;
}

namespace ICEAVGLUMPROC_NS
{

class ICEAVGLUMPROC_EXPORT ItemWidget : public QWidget
{
	Q_OBJECT
public:
	ItemWidget(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~ItemWidget();

	void init(int index, const ICEPROC_NS::ImageProcInterface* proc,
		ICEPROC_NS::ImageWindowInterface* imageWindowInterface, const QColor& color,
		const QColor& redColor, const QColor& greenColor, const QColor& blueColor);
	void setCheckable(bool checkable);
	void setTitle(const QString& name);
	void clear();
	void setMaskPBEnabled(bool enabled);
	void setParamObject(const QSharedPointer<QObject>& param, bool fromDb = false);
	QSharedPointer<QObject> getParamObject() const;
	void setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView() const;
	QString getMaskName() const;
	void calcY();
	bool isCanUsed() const;

protected slots:
	void maskGB_toggled(bool on);
	void maskCalcPB_clicked();
	void maskCancelPB_clicked();
	void maskAutoKPB_clicked();
	void maskClearKPB_clicked();
	void yKMaskDSB_valueChanged(double value);
	void yKMaskChB_stateChanged(int state);
	void onImageBaseValueChanged(int imageBaseValue);
	void onMaskPBEnabled(bool enabled);
	void onChannelTypeChanged(int channelType);
	void onParamFilterBaseChanged(QSharedPointer<QObject> paramFilterBase);

signals:
	void maskCreated(int index, const QString& name);
	void maskRemoved(int index);
	void maskChanged();

private:
	void setColor(const QColor& color);

	Ui::ItemWidget ui;	///< Ui класс

	int m_index;
	const ICEPROC_NS::ImageProcInterface* m_proc;
	ICEPROC_NS::ImageWindowInterface* m_imageWindowInterface;
	QColor m_redColor;
	QColor m_greenColor;
	QColor m_blueColor;
	double m_maxKDeltaValue;
	double m_minKDeltaValue;
	int m_imageBaseValue;
	Param::YRGBChannelType m_channelType;
	QSharedPointer<QObject> m_paramFilterBase;
	QSharedPointer<QObject> m_param;
	bool m_fromDb;
	bool m_maskCalced;
};

} // namespace ICEAVGLUMPROC_NS

#endif // ICEAVGLUMPROC_ITEMWIDGET_H
