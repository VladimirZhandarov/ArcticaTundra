#ifndef PROCESSINGWIDGET_H
#define PROCESSINGWIDGET_H

#include "iceguidef.h"

#include <QtGui/QWidget>

#include <boost/tr1/memory.hpp>

namespace ICEODB_NS
{
	class FilterParam;
}

namespace ICEGUI_NS
{

/** Виджет отображения процесса применения фильтра.
    Состоит из трех прогресс-баров, которые отображают
    процесс обработки файла, применение маски фильтра,
    применение самого фильтра (если он групповой)
*/
class ICEGUI_EXPORT ProcessingWidget : public QWidget
{
	Q_OBJECT
public:
	ProcessingWidget(const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam,
		QWidget* parent = 0, Qt::WindowFlags f = 0);
	~ProcessingWidget();

	void setImageCount(int);
	void setImageNum(int imageNum, const QString& imageName = QString::null);

protected slots:
	void onMaskProgressRangeChanged(int minimum, int maximum);
	void onMaskProgressValueChanged(int progressValue);
	void onMaskProgressTextChanged(const QString& progressText);
	void onFilterCalculated(int filterNum, const QString& filterType);

private:
	void setFilterCount(int);

private:
	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEGUI_NS

#endif // PROCESSINGWIDGET_H