//==============================================================================
// Название:        Редактор изображения
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: shchav $
// Создан:    2011.11.08
// Изменен:   $Date: 2013-04-19 18:46:52 +0400 (РџС‚, 19 Р°РїСЂ 2013) $
// Модуль:    icegui
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: imageeditor.h
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include "iceqwtdef.h"

#include <QtGui/QFrame>

#include <boost/tr1/memory.hpp>

namespace ICEODB_NS
{
	class MaskView;
}

namespace ICEIMAGE_NS
{
	class ImageBuilder;
}

namespace ICEQWT_NS
{
	class Raster;
	class IceQwtPlot;

class ICEQWT_EXPORT ImageEditor : public QFrame
{
	Q_OBJECT
public:
	//TODO refac
	enum MODE
	{
		PICKER_RECT = 0,
		PICKER_ELLIPSE,
		PICKER_POLYGON,
		PICKER_POLYLINE,
		MOVE,
		NOMODE
	};

	ImageEditor(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~ImageEditor();

	bool load(const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>&
		backgroundImageMaskView);
	bool load(const QString& filename);
	bool save(const QString& filename, int quality);
	const QString& getFilename() const;

	// Сетка
	bool gridVisible() const;
	void setGridVisible(bool tf);

	// Оси
	bool axisVisible() const;
	void setAxisVisible(bool tf);

	//virtual void initialiaze();

	QSharedPointer<ICEIMAGE_NS::ImageBuilder> getImageBuilder() const;

	// Масштаб
	int getPercentScaleInWidth();
	int getPercentScaleInHeight();
	int getPercentScaleInSize();
	void setPercentScale(int);
	int getCurrentPercentScale();

	// Режим
	void setMode(MODE);
	MODE getMode() const;
	MODE getLastPickerMode() const;
	MODE getLastNavigationMode() const;

	void cleanSelectedArea();

	//TODO obsolete
	QVariant getSelectedArea();

	// TODO если возвращаем plot, то незачем ф-ии bool gridVisible() const;,
	// так как это можно узнать и от plotы
	//TODO refactor не красивое решение
	IceQwtPlot* icePlot() const;

signals:
	// Масштабы
	void zoomIn();
	void zoomOut();

	void errorProcessed(QString errorMsg);
	void windowChangeModified();
	void cursorPositionChanged(); //TODO refac
	void mouseDoubleClicked(); //TODO refac
	void selectedAreaChanged();

protected slots:
	virtual void onImageReady();
	void onCursorPositionChanged(const QPoint& pos);
	void onMouseDoubleClicked(const QPoint& pos);
	void onSelectedAreaChanged();

private:
	void init();

	class PrivateData;
	PrivateData* d_data;
};

} // namespace ICEQWT_NS

#endif // IMAGEEDITOR_H
