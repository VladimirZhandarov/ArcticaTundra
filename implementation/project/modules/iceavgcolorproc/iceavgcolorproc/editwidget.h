#ifndef ICEAVGCOLORPROC_EDITWIDGET_H
#define ICEAVGCOLORPROC_EDITWIDGET_H

#include "iceavgcolorprocdef.h"

#include <iceproc/editwidget.h>

#include "ui_avgcoloreditwidget.h"

namespace ICEAVGCOLORPROC_NS
{

class ICEAVGCOLORPROC_EXPORT EditWidget : public ICEPROC_NS::EditWidget
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
	virtual QString getMaskName(int index) const;
	virtual bool isWithImages() const;

private:
	Ui::AvgColorEditWidget ui;	///< Ui класс
};

} // namespace ICEAVGCOLORPROC_NS

#endif // ICEAVGCOLORPROC_EDITWIDGET_H
