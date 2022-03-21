#ifndef SECTIONEDITDIALOG_H
#define SECTIONEDITDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_sectioneditdialog.h"

#include <boost/tr1/memory.hpp>

namespace ICECORE_NS
{
	class FilterItemModel;
}

namespace ICEODB_NS
{
	class FilterSection;
}

namespace ICEGUI_NS
{

/**
* @class SectionEditDialog
* Окно редактирования раздела в базе данных
*/
class ICEGUI_EXPORT SectionEditDialog : public QDialog
{
	Q_OBJECT
public:
	/**
	* Конструктор
	* @param filterModel TODO comment
	* @param parentModelIndex TODO comment
	* @param section выбранный раздел
	* @param parent родительский объект
	* @param f флаги окна
	*/
	SectionEditDialog(ICECORE_NS::FilterItemModel* filterModel,
		const QModelIndex& parentModelIndex,
		const std::tr1::shared_ptr<ICEODB_NS::FilterSection>& section,
		QWidget* parent = 0, Qt::WindowFlags f = 0);

	/**
	* Деструктор
	*/
	~SectionEditDialog();

	std::tr1::shared_ptr<ICEODB_NS::FilterSection> getResultSection() const;

protected slots:
	void okPB_clicked();

private:
	/**
	* Установить признак редактирования
	* @param editType признак редактирования
	*/
	void setEditType(bool editType);

	Ui::SectionEditDialog ui;	///< Ui класс

	ICECORE_NS::FilterItemModel* m_filterModel;
	QModelIndex m_parentModelIndex;
	std::tr1::shared_ptr<ICEODB_NS::FilterSection> m_resultSection; ///< Созданный раздел
};

} // namespace ICEGUI_NS

#endif // SECTIONEDITDIALOG_H
