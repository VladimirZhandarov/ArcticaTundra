#ifndef ICEFINDOBJECTPROC_EDITWIDGET_H
#define ICEFINDOBJECTPROC_EDITWIDGET_H

#include "icefindobjectprocdef.h"

#include <iceproc/editwidget.h>

#include "ui_findobjecteditwidget.h"

namespace ICEFINDOBJECTPROC_NS
{

class ICEFINDOBJECTPROC_EXPORT EditWidget : public ICEPROC_NS::EditWidget
{
	Q_OBJECT
public:
	EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
		QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~EditWidget();

	virtual bool isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder) const;
	virtual void setFilterParamId(unsigned int filterParamId);
	virtual void setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb = false);
	virtual QSharedPointer<QObject> getParamObject(int index) const;
	virtual void setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	virtual std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView(int index) const;
	virtual void setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);
	virtual std::tr1::shared_ptr<ICEODB_NS::LineView> getLineView(int index) const;

private:
	Ui::FindObjectEditWidget ui;	///< Ui класс
};

} // namespace ICEFINDOBJECTPROC_NS

#endif // ICEFINDOBJECTPROC_EDITWIDGET_H
