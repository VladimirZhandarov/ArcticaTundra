#include "sectioneditdialog.h"

#include <QtCore/QAbstractItemModel>

#include <QtGui/QMessageBox>

#include <iceodb/odb/filtersection.hxx>
#include <icecore/filteritemmodel.h>

namespace ICEGUI_NS
{

SectionEditDialog::SectionEditDialog(ICECORE_NS::FilterItemModel* filterModel,
	const QModelIndex& parentModelIndex, const std::tr1::shared_ptr<ICEODB_NS::FilterSection>& section,
	QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_filterModel(filterModel),
	m_parentModelIndex(parentModelIndex), m_resultSection(section)
{
	ui.setupUi(this);
	setEditType(m_resultSection.get() != 0);
}

SectionEditDialog::~SectionEditDialog()
{

}

void SectionEditDialog::setEditType(bool editType)
{
	if(editType && m_resultSection.get())
	{
		setWindowTitle(tr("Изменить данные раздела"));
		ui.nameLE->setText(m_resultSection->getName());
		ui.descriptionTE->setPlainText(m_resultSection->getDescription());
	}
	else
	{
		setWindowTitle(tr("Добавить новый раздел"));
		ui.nameLE->clear();
		ui.descriptionTE->clear();
	}
}

std::tr1::shared_ptr<ICEODB_NS::FilterSection> SectionEditDialog::getResultSection() const
{
	return m_resultSection;
}

void SectionEditDialog::okPB_clicked()
{
	Q_ASSERT(m_filterModel);
	unsigned int filterId = 0;
	if(m_resultSection.get())
	{
		filterId = m_resultSection->getId();
	}
	QModelIndex duplicateModelIndex = m_filterModel->getModelIndexOfDuplicateFilterName(
		ui.nameLE->text(), filterId, "", m_parentModelIndex);
	if(duplicateModelIndex.isValid())
	{
		QMessageBox::warning(this, tr("Неверное имя раздела"),
			tr("Раздел с указанным именем уже существует"));
		return;
	}
	if(m_resultSection.get())
	{
		m_resultSection->setName(ui.nameLE->text());
		m_resultSection->setDescription(ui.descriptionTE->toPlainText());
	}
	else
	{
		m_resultSection = std::tr1::shared_ptr<ICEODB_NS::FilterSection>(
			new ICEODB_NS::FilterSection(ui.nameLE->text(),
			ui.descriptionTE->toPlainText()));
	}
	QDialog::accept();
}

} // namespace ICEGUI_NS
