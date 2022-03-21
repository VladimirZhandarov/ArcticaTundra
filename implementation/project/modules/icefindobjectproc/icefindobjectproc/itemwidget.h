#ifndef ICEFINDOBJECTPROC_ITEMWIDGET_H
#define ICEFINDOBJECTPROC_ITEMWIDGET_H

#include "icefindobjectprocdef.h"

#include <QtGui/QWidget>

#include "ui_itemwidget.h"

#include <boost/tr1/memory.hpp>

namespace ICEODB_NS
{
	class MaskView;
}

namespace ICEFINDOBJECTPROC_NS
{

class ICEFINDOBJECTPROC_EXPORT ItemWidget : public QWidget
{
	Q_OBJECT
public:
	ItemWidget(QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~ItemWidget();

	void init(int index, const QColor& lineColor, const QColor& maskColor);
	void setCheckable(bool checkable);
	void setTitle(const QString& name);
	void clear();
	void setParamObject(const QSharedPointer<QObject>& param, bool fromDb = false);
	QSharedPointer<QObject> getParamObject() const;
	void setMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView() const;
	void setLineView(const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);
	std::tr1::shared_ptr<ICEODB_NS::LineView> getLineView() const;
	QString getMaskName() const;

protected slots:
	void minAreaChB_stateChanged(int state);
	void minPerimeterChB_stateChanged(int state);
	void minKFormChB_stateChanged(int state);

private:
	void setColor(const QColor& color, bool forLine = false);

	Ui::ItemWidget ui;	///< Ui класс

	int m_index;
};

} // namespace ICEFINDOBJECTPROC_NS

#endif // ICEFINDOBJECTPROC_ITEMWIDGET_H
