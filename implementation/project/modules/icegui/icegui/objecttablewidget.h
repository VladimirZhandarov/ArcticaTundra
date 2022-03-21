#ifndef OBJECTTABLEWIDGET_H
#define OBJECTTABLEWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>
#include "ui_objecttablewidget.h"

#include <icecore/settings.h>

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEQWT_NS
{
	class IceUnitRatioQwtTransform;
}

namespace ICEGUI_NS
{

class ICEGUI_EXPORT ObjectTableWidget : public QWidget
{
	Q_OBJECT

public:
	ObjectTableWidget(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
		int index, QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ObjectTableWidget();

	bool isNull() const;
	QString getFilterName() const;
	QString getName() const;
	void updateState(ICEQWT_NS::IceUnitRatioQwtTransform* transformation, bool halfArcLength);
	void currentRowChange();

	static QString createHtmlFile(const QStringList& fileNameList, const QString& destinationDir,
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder, ICEIMAGE_NS::UNIT unit,
		double divK, bool halfArcLength, const QList<ICEIMAGE_NS::ContourItemModel*>& summaryContourItemModelList
		= QList<ICEIMAGE_NS::ContourItemModel*>());

	static QString createCsvFiles(const QString& filename, const QString& destinationDir,
		QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder);

	static bool createCsvFile(const QString& filename, ICEIMAGE_NS::ContourItemModel* contourItemModel,
		QAbstractItemModel* dataModel, const QString& imageFilename, const QSize& imageSize);

protected slots:
	void onCurrentContourDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void onTreeViewClicked(const QModelIndex& index);
	void saveToFilePB_clicked();
	void onSelectAll();
	void onUnselectAll();

private:
	static QString tabHtmlLine(const QString& line, unsigned int tabNumber);

	Ui::ObjectTableWidget ui;	///< Ui класс

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> m_imageBuilder;
	int m_index;
	ICEQWT_NS::IceUnitRatioQwtTransform* m_transformation;
	bool m_halfArcLength;
};

} // namespace ICEGUI_NS

#endif // OBJECTTABLEWIDGET_H
