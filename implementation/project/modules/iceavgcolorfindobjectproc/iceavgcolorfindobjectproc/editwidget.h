#ifndef ICEAVGCOLORFINDOBJECTPROC_EDITWIDGET_H
#define ICEAVGCOLORFINDOBJECTPROC_EDITWIDGET_H

#include "iceavgcolorfindobjectprocdef.h"

#include <iceproc/editwidget.h>

#include "ui_avgcolorfindobjecteditwidget.h"

namespace ICEAVGCOLORFINDOBJECTPROC_NS
{

class ICEAVGCOLORFINDOBJECTPROC_EXPORT EditWidget : public ICEPROC_NS::EditWidget
{
	Q_OBJECT
public:
	EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
		QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~EditWidget();

	virtual void setFilterParamId(unsigned int filterParamId);
	virtual void setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb = false);
	virtual QSharedPointer<QObject> getParamObject(int index) const;
	virtual void setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	virtual std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView(int index) const;
	virtual void setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);
	virtual std::tr1::shared_ptr<ICEODB_NS::LineView> getLineView(int index) const;
	virtual QString getMaskName(int index) const;
	virtual bool isWithImages() const;

private:
	Ui::AvgColorFindObjectEditWidget ui;	///< Ui класс

};

} // namespace ICEAVGCOLORFINDOBJECTPROC_NS

#endif // ICEAVGCOLORFINDOBJECTPROC_EDITWIDGET_H
