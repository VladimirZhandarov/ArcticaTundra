#ifndef ICEAVGCOLORPROC_ITEMWIDGET_H
#define ICEAVGCOLORPROC_ITEMWIDGET_H

#include "iceavgcolorprocdef.h"

#include <QtGui/QWidget>

#include "ui_itemwidget.h"

#include "param.h"

namespace ICEPROC_NS
{
	class ImageProcInterface;
	class ImageWindowInterface;
}

namespace ICEAVGCOLORPROC_NS
{

class ICEAVGCOLORPROC_EXPORT ItemWidget : public QWidget
{
	Q_OBJECT
public:
	ItemWidget(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~ItemWidget();

	void init(int index, const ICEPROC_NS::ImageProcInterface* proc,
		ICEPROC_NS::ImageWindowInterface* imageWindowInterface, const QColor& color);
	void setCheckable(bool checkable);
	void setTitle(const QString& name);
	void clear();
	void setMaskPBEnabled(bool flag);
	void setParamObject(const QSharedPointer<QObject>& param, bool fromDb = false);
	QSharedPointer<QObject> getParamObject() const;
	void setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView() const;
	QString getMaskName() const;

protected slots:
	void maskCalcPB_clicked();
	void maskCancelPB_clicked();
	void maskAutoKPB_clicked();
	void maskClearKPB_clicked();
	void rgbKMaskDSB_valueChanged(double value);
	void rgbKMaskChB_stateChanged(int state);
	void dontUseBlackChB_stateChanged(int state);
	void dontUseWhiteChB_stateChanged(int state);

signals:
	void maskCreated(int index, const QString& name);
	void maskRemoved(int index);

private:
	void setColor(const QColor& color);

	Ui::ItemWidget ui;	///< Ui класс

	int m_index;
	const ICEPROC_NS::ImageProcInterface* m_proc;
	ICEPROC_NS::ImageWindowInterface* m_imageWindowInterface;
	QColor m_redColor;
	QColor m_greenColor;
	QColor m_blueColor;
	double m_upRDeltaValue;
	double m_downRDeltaValue;
	double m_upGDeltaValue;
	double m_downGDeltaValue;
	double m_upBDeltaValue;
	double m_downBDeltaValue;
	QSharedPointer<QObject> m_param;
};

} // namespace ICEAVGCOLORPROC_NS

#endif // ICEAVGCOLORPROC_ITEMWIDGET_H
