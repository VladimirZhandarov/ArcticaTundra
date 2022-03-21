#ifndef FILTEREDITDIALOG_H
#define FILTEREDITDIALOG_H

#include "iceguidef.h"

#include <QtGui/QDialog>
#include "ui_filtereditdialog.h"

#include <boost/tr1/memory.hpp>

#include <QtCore/QMap>

#include <iceimage/imagebuilder.h>

namespace ICEPROC_NS
{
	class Param;
	class EditWidget;
	class ImageProcInterface;
	class ImageWindowInterface;
}

namespace ICEODB_NS
{
	class FilterParam;
	class MaskView;
	class LineView;
}

namespace ICECORE_NS
{
	class FilterItemModel;
}

namespace ICEGUI_NS
{

/**
* @class FilterEditDialog
* Окно создания фильтров
*/
//TODO поделить на gui и nongui части
class ICEGUI_EXPORT FilterEditDialog : public QDialog
{
	Q_OBJECT
protected:
	/**
	* Конструктор
	* @param filterParamModelIndex TODO comment
	* @param proc алгоритмом обработки изображения
	* @param imageWindowInterface интерфейс главного окна приложения
	* @param parent родительский объект
	* @param f флаги окна
	*/
	FilterEditDialog(const QModelIndex& filterParamModelIndex, ICEPROC_NS::ImageProcInterface* proc,
		ICEPROC_NS::ImageWindowInterface* imageWindowInterface, QWidget* parent = 0,
		Qt::WindowFlags f = 0);

	FilterEditDialog* getFilterEditDialog(unsigned int filterParamId) const;
	FilterEditDialog* createFilterEditDialog(const QModelIndex& filterParamModelIndex,
		QWidget* parent = 0, Qt::WindowFlags f = 0);

public:
	/**
	* Деструктор
	*/
	~FilterEditDialog();

	//TODO refac
	FilterEditDialog(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
		ICEPROC_NS::ImageProcInterface* proc, QWidget* parent = 0, Qt::WindowFlags f = 0);
	//TODO refac protected
	void init();

	unsigned int getFilterParamId() const;
	QString getName() const;
	const QString& getDllName() const;
	void setUseFilterPB(bool enabled);

	//TODO refac
	bool prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder =
		QSharedPointer<ICEIMAGE_NS::ImageBuilder>());
	//TODO refac
	bool isDbConnected() const;

	QVector<QSharedPointer<QObject> > getParamVector() const;
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > getMaskViewVector() const;
	QVector<std::tr1::shared_ptr<ICEODB_NS::LineView> > getLineViewVector() const;
	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& getBackgroundMaskView() const;

	static void clearFilterEditDialogMap();

	void setImage(int index, const QString& name, const QString& filename, const QImage& image);
	void setModel(ICECORE_NS::FilterItemModel* filterModel);
	void showDialog();

	void initData();
	void show(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam =
		std::tr1::shared_ptr<ICEODB_NS::FilterParam>(), bool filterCanBeUsed = true);

	static FilterEditDialog* createInstance(ICEPROC_NS::ImageProcInterface* proc,
		ICEPROC_NS::ImageWindowInterface* imageWindowInterface,
		ICECORE_NS::FilterItemModel* filterItemModel, bool createDialog,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	static FilterEditDialog* createInstance(const QModelIndex& filterParamModelIndex,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	static FilterEditDialog* getInstance(const QString& key, unsigned int filterParamId = 0);

signals:
	void started(ICEGUI_NS::FilterEditDialog* dialog);
	void finished(ICEGUI_NS::FilterEditDialog* dialog);
	void filterParamCreated(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam);
	void currentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);
	void databaseConnectionChanged(bool connected);

protected:
	void closeEvent(QCloseEvent* event);
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> createFilterParam() const;
	bool isFilterParamChanged() const;

public slots:
	void onDatabaseConnectionChanged(bool connected);

protected slots:
	void onOpenDialogFinished(ICEGUI_NS::FilterEditDialog* dialog);
	void onCurrentImageBuilderChanged(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder);
	void nameLE_textChanged(const QString& text);
	void showImagePB_clicked(bool checked);
	void useFilterPB_clicked();
	void saveFilterToFilePB_clicked();
	void saveFilterPB_clicked();

	void onMaskCreated(int index, const QString& name);
	void onMaskRemoved(int index);

private:
	void initSize();

	Ui::FilterEditDialog ui;	///< Ui класс

	ICEPROC_NS::ImageProcInterface* m_proc;
	ICEPROC_NS::ImageWindowInterface* m_imageWindowInterface;
	ICEPROC_NS::EditWidget* m_contentWidget;
	ICECORE_NS::FilterItemModel* m_filterModel;
	QModelIndex m_filterParamModelIndex;

	static QMap<QString, ICEGUI_NS::FilterEditDialog*> createFilterEditDialogMap;
	QMap<unsigned int, ICEGUI_NS::FilterEditDialog*> m_openFilterEditDialogMap;
	int m_contentWidgetWidth;
	int m_imageWidgetWidth;
	bool m_databaseConnected;

	std::tr1::shared_ptr<ICEODB_NS::FilterParam> m_filterParam;
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> m_filterParamFromDb;
};

} // namespace ICEGUI_NS

#endif // FILTERCREATEDIALOG_H
