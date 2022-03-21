#ifndef ICEPROC_EDITWIDGET_H
#define ICEPROC_EDITWIDGET_H

#include "iceprocdef.h"

#include <QtGui/QWidget>

#include "ui_editwidget.h"

#include <boost/tr1/memory.hpp>

#include <iceimage/imagebuilder.h>

class QSpinBox;
class QDoubleSpinBox;

namespace ICEODB_NS
{
	class FilterParam;
	class MaskParam;
	class MaskView;
	class LineView;
}

namespace ICEPROC_NS
{
	class ImageProcInterface;
	class ImageWindowInterface;

/**
* @class EditWidget
* ���� ��� �������������� ���������� ��������� ��� ��������� �����������
*/
class ICEPROC_EXPORT EditWidget : public QWidget
{
	Q_OBJECT
public:
	/**
	* �����������
	* @param proc plugin ��������� ��� ��������� �����������
	* @param imageWindowInterface ������� ���� ����������
	* @param maskParamCount ���������� �������
	* @param useUi ������������ ui ���� ����� �������
	*              ���� =False, �� ui ���� ������������ � ������-����������
	* @param parent ������������ ������
	*/
	EditWidget(const ImageProcInterface* proc, ImageWindowInterface* imageWindowInterface,
		int maskParamCount, bool useUi = false, QWidget* parent = 0);

	/**
	* ����������
	*/
	virtual ~EditWidget();

	/**
	* �������� ����
	*/
	virtual void clear();

	//TODO refac
	virtual int setChildFilterParam(int index, const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);
	virtual int getChildFilterParamCount() const;
	virtual std::tr1::shared_ptr<ICEODB_NS::FilterParam> getChildFilterParam(int index) const;

	void setMaskParam(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskParam>& maskParam);
	int getMaskParamCount() const;
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> getMaskParam(int index) const;

	virtual bool isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder) const;
	virtual void setFilterParamId(unsigned int filterParamId);
	virtual void setParamObject(int index, const QSharedPointer<QObject>& param, bool fromDb = false);
	virtual QSharedPointer<QObject> getParamObject(int index) const;
	virtual void setMaskView(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView);
	virtual std::tr1::shared_ptr<ICEODB_NS::MaskView> getMaskView(int index) const;
	virtual void setLineView(int index, const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView);
	virtual std::tr1::shared_ptr<ICEODB_NS::LineView> getLineView(int index) const;
	virtual QString getMaskName(int index) const;
	virtual bool isWithImages() const;
	virtual bool prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder =
		QSharedPointer<ICEIMAGE_NS::ImageBuilder>());

signals:
	void maskCreated(int index, const QString& name);
	void maskRemoved(int index);
	void currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);

protected:
	Ui::EditWidget ui;	///< Ui �����

	bool m_useUi;		///< ������������ ui ���� ����� �������
	const ImageProcInterface* m_proc;
	ImageWindowInterface* m_imageWindowInterface;

	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskParam> > m_maskParamVector;
	unsigned int m_filterParamId;
};

} // namespace ICEPROC_NS

#endif // ICEPROC_EDITWIDGET_H
