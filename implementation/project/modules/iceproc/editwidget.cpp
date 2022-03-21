#include "editwidget.h"

#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>

#include "doublespinboxdelegate.h"
#include "imageprocinterface.h"

#include <iceodb/odb/maskparam.hxx>

namespace ICEPROC_NS
{

EditWidget::EditWidget(const ImageProcInterface* proc, ImageWindowInterface* imageWindowInterface,
	int maskParamCount, bool useUi, QWidget* parent) : QWidget(parent), m_proc(proc),
	m_imageWindowInterface(imageWindowInterface), m_useUi(useUi), m_filterParamId(0)
{
	if(m_useUi)
	{
		ui.setupUi(this);
	}
	if(maskParamCount > 0)
	{
		m_maskParamVector.resize(maskParamCount);
		for(int i=0; i<maskParamCount; i++)
		{
			m_maskParamVector[i] = std::tr1::shared_ptr<ICEODB_NS::MaskParam>(
				new ICEODB_NS::MaskParam());
		}
	}
}

EditWidget::~EditWidget()
{

}

void EditWidget::clear()
{
	return;
}

bool EditWidget::isCanUsed(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder) const
{
	return !imageBuilder.isNull();
}

void EditWidget::setFilterParamId(unsigned int filterParamId)
{
	m_filterParamId = filterParamId;
}

int EditWidget::setChildFilterParam(int /*index*/,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& /*filterParam*/)
{
	return 0;
}

int EditWidget::getChildFilterParamCount() const
{
	return 0;
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> EditWidget::getChildFilterParam(int /*index*/) const
{
	return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
}

void EditWidget::setMaskParam(int index, const std::tr1::shared_ptr<ICEODB_NS::MaskParam>& maskParam)
{
	if(index >= 0 && index < m_maskParamVector.size() && maskParam.get())
	{
		m_maskParamVector[index] = maskParam;
		Q_ASSERT(m_proc);
		QSharedPointer<QObject> param = m_proc->fromJSonString(index, maskParam->getJSonParam());
		setParamObject(index, param, true);
		setMaskView(index, maskParam->getMaskView());
		setLineView(index, maskParam->getLineView());
	}
}

int EditWidget::getMaskParamCount() const
{
	return m_maskParamVector.size();
}

std::tr1::shared_ptr<ICEODB_NS::MaskParam> EditWidget::getMaskParam(int index) const
{
	if(index >=0 && index < m_maskParamVector.size())
	{
		QSharedPointer<QObject> param = getParamObject(index);
		Q_ASSERT(m_proc);
		m_maskParamVector[index]->setJSonParam(m_proc->toJSonString(param));
		m_maskParamVector[index]->setMaskView(getMaskView(index));
		m_maskParamVector[index]->setLineView(getLineView(index));
		return m_maskParamVector[index];
	}
	else
	{
		return std::tr1::shared_ptr<ICEODB_NS::MaskParam>();
	}
}

void EditWidget::setParamObject(int /*index*/, const QSharedPointer<QObject>& /*param*/, bool /*fromDb*/)
{
	return;
}

QSharedPointer<QObject> EditWidget::getParamObject(int /*index*/) const
{
	return QSharedPointer<QObject>();
}

void EditWidget::setMaskView(int /*index*/, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& /*maskView*/)
{

}

std::tr1::shared_ptr<ICEODB_NS::MaskView> EditWidget::getMaskView(int /*index*/) const
{
	return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
}

void EditWidget::setLineView(int /*index*/, const std::tr1::shared_ptr<ICEODB_NS::LineView>& /*lineView*/)
{

}

std::tr1::shared_ptr<ICEODB_NS::LineView> EditWidget::getLineView(int /*index*/) const
{
	return std::tr1::shared_ptr<ICEODB_NS::LineView>();
}

QString EditWidget::getMaskName(int /*index*/) const
{
	return "";
}

bool EditWidget::isWithImages() const
{
	return false;
}

bool EditWidget::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& /*imageBuilder*/)
{
	return true;
}

} // namespace ICEPROC_NS
