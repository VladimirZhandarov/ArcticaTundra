#ifndef ICEGROUPPROC_EDITWIDGET_H
#define ICEGROUPPROC_EDITWIDGET_H

#include "icegroupprocdef.h"

#include <iceproc/editwidget.h>

#include "ui_groupeditwidget.h"

#include <QtGui/QTreeWidgetItem>

namespace ICEGROUPPROC_NS
{

class ICEGROUPPROC_EXPORT EditWidget : public ICEPROC_NS::EditWidget
{
	Q_OBJECT
public:
	EditWidget(const ICEPROC_NS::ImageProcInterface* proc, ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
		QWidget* parent = 0);

	/**
	* Деструктор
	*/
	~EditWidget();

	//TODO refac
	virtual int setChildFilterParam(int index, const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);
	virtual int getChildFilterParamCount() const;
	virtual std::tr1::shared_ptr<ICEODB_NS::FilterParam> getChildFilterParam(int index) const;

	virtual bool isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder) const;
	virtual void setFilterParamId(unsigned int filterParamId);
	virtual void setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb = false);
	virtual QSharedPointer<QObject> getParamObject(int index) const;
	virtual void setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	virtual std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView(int index) const;
	virtual void setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);
	virtual std::tr1::shared_ptr<ICEODB_NS::LineView> getLineView(int index) const;
	virtual QString getMaskName(int index) const;
	virtual bool prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder =
		QSharedPointer<ICEIMAGE_NS::ImageBuilder>());

private:
	Ui::GroupEditWidget ui;	///< Ui класс
};

} // namespace ICEGROUPPROC_NS

#endif // ICEAGROUPPROC_EDITWIDGET_H
