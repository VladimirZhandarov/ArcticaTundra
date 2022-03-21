//==============================================================================
// Название:        Редактор изображения
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: shchav $
// Создан:    2011.11.08
// Изменен:   $Date: 2013-04-19 18:46:52 +0400 (РџС‚, 19 Р°РїСЂ 2013) $
// Модуль:    iceqwt
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: imageeditor.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "imageeditor.h"

#include "iceqwtplot.h"
#include "iceqwtplotrasteritem.h"
#include "iceqwtscrollplotmagnifier.h"
#include "iceqwtplotpicker.h"
#include "iceqwtplotpanner.h"

#include <iceimage/gdalimagebuilder.h>

namespace ICEQWT_NS
{

class ImageEditor::PrivateData
{
public:
	PrivateData() : icePlot(0), mode(NOMODE)
	{

	}
	IceQwtPlot* icePlot;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder;

	MODE mode;
	MODE lastPickerMode;
	MODE lastNavigationMode;
};

//! Конструктор
ImageEditor::ImageEditor(QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f)
{
	init();
}

//! Деструктор
ImageEditor::~ImageEditor()
{
	d_data->imageBuilder->quit();
	d_data->imageBuilder->wait();
	delete d_data;
}

/*!
  \brief Инициализация ImageEditor
 */
void ImageEditor::init()
{
	d_data = new PrivateData;

	d_data->icePlot = new IceQwtPlot(this);
	d_data->icePlot->setMouseTracking(true);
	d_data->imageBuilder = QSharedPointer<ICEIMAGE_NS::ImageBuilder>(new ICEIMAGE_NS::GdalImageBuilder());
	connect(d_data->imageBuilder.data(), SIGNAL(imageReady()), this, SLOT(onImageReady()));
	connect(d_data->imageBuilder.data(), SIGNAL(errorProcessed(QString)), this, SIGNAL(errorProcessed(QString)));

	d_data->icePlot->rasterItem()->setImageBuilder(d_data->imageBuilder);
	d_data->icePlot->magnifier()->setImageBuilder(d_data->imageBuilder);
	connect(d_data->icePlot->magnifier(), SIGNAL(zoomIn()), this, SIGNAL(zoomIn()));
	connect(d_data->icePlot->magnifier(), SIGNAL(zoomOut()), this, SIGNAL(zoomOut()));

	connect(d_data->icePlot->canvas(), SIGNAL(cursorPositionChanged(const QPoint&)),
		this, SLOT(onCursorPositionChanged(const QPoint&)));
	connect(d_data->icePlot->canvas(), SIGNAL(mouseDoubleClicked(const QPoint&)),
		this, SLOT(onMouseDoubleClicked(const QPoint&)));
	connect(d_data->icePlot->picker(), SIGNAL(selectedAreaChanged()), this, SLOT(onSelectedAreaChanged()));
}

//! Получение обработчика изображения
QSharedPointer<ICEIMAGE_NS::ImageBuilder> ImageEditor::getImageBuilder() const
{
	return d_data->imageBuilder;
}

/*!
  \brief Загрузить изображение
  Любой редактор может загрузить изображение
  \param image загружаемое изображение
*/
bool ImageEditor::load(const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView)
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	if(imageBuilder->load(image))
	{
		icePlot()->setup();
		imageBuilder->setBackgroundImageMaskView(backgroundImageMaskView);
		// TODO Надо проверить на факт сохранение уже
		// отображаемого изображения и предложить пользователю
		// его сохранить
		setWindowModified(false);
		return true;
	}
	else
	{
		return false;
	}
}

bool ImageEditor::load(const QString& filename)
{
	if(getImageBuilder()->load(filename))
	{
		//TODO разобраться с setup()
		icePlot()->setup();
		setWindowModified(false);
		return true;
	}
	else
	{
		return false;
	}
}

/*!
  \brief Сохранение файла
  Изображение, отображаемое в редакторе может сохранить
  любой редактор.
  \param filename имя сохраняемого файла
*/
bool ImageEditor::save(const QString& filename, int quality)
{
	getImageBuilder()->save(filename, quality);
	setWindowModified(false);
	return true;
}

/*!
  \brief Получение имени файла

  Получение имени файла, изображение которого отображается.
  Если изображение не было открыто из файла или не было
  сохранено в файл, то вернется QString::Null
*/
const QString& ImageEditor::getFilename() const
{
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = getImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	return imageBuilder->getFilename();
}

// \return Возвращает плот
IceQwtPlot* ImageEditor::icePlot() const
{
	return d_data->icePlot;
}

//! Обработчик факта изменения отображения
void ImageEditor::onImageReady()
{
	d_data->icePlot->replot();
	setWindowModified(true);
	//emit windowChangeModified();
}

void ImageEditor::onCursorPositionChanged(const QPoint& pos)
{
	Q_ASSERT(!d_data->imageBuilder.isNull());
	d_data->imageBuilder->setCursorPosition(pos);
	emit cursorPositionChanged();
}

void ImageEditor::onMouseDoubleClicked(const QPoint& pos)
{
	Q_ASSERT(!d_data->imageBuilder.isNull());
	if(d_data->imageBuilder->setCurrentContourByPos(pos))
	{
		emit mouseDoubleClicked();
	}
}

void ImageEditor::onSelectedAreaChanged()
{
	Q_ASSERT(!d_data->imageBuilder.isNull());
	d_data->imageBuilder->setUserCreatedObject(icePlot()->picker()->getSelectedArea());
	emit selectedAreaChanged();
}

/*!
  \return \c True, если сетка отображается
*/
bool ImageEditor::gridVisible() const
{
	return d_data->icePlot->gridVisible();
}

/*!
  Отобразить/скрыть сетку
  \param tf флаг отображения/скрытия сетки
*/
void ImageEditor::setGridVisible(bool tf)
{
	d_data->icePlot->setGridVisible(tf);
}

/*!
  \return \c True, если оси отображаются
*/
bool ImageEditor::axisVisible() const
{
	return d_data->icePlot->axisVisible();
}

/*!
  Отобразить/скрыть оси
  \param tf флаг отображения/скрытия осей
*/
void ImageEditor::setAxisVisible(bool tf)
{
	d_data->icePlot->setAxisVisible(tf);
}

/*!
  \brief Инициализация редактора 
  Инициализировать редактор после первого отображения редактора
*/
//void ImageEditor::initialiaze()
//{
//	icePlot()->initAxis();
//}

/*!
  \brief Установить масштаб в процентах
*/ 
void ImageEditor::setPercentScale(int percentScale)
{
	icePlot()->magnifier()->setPercentScale(percentScale);
}

/*!
  \brief Получить текущий масштаб изображения в процентах
*/ 
int ImageEditor::getCurrentPercentScale()
{
	return icePlot()->magnifier()->getCurrentPercentScale();
}

/*!
  \brief Получить масштаб в процентах при котором изображение
         вписывается в окно
*/ 
int ImageEditor::getPercentScaleInSize()
{
	return icePlot()->magnifier()->getPercentScaleInSize();
}

/*!
  \brief Получить масштаб в процентах при котором изображение
         вписывается по высоте
*/
int ImageEditor::getPercentScaleInHeight()
{
	return icePlot()->magnifier()->getPercentScaleInHeight();
}

/*!
  \brief Получить масштаб в процентах при котором изображение
         вписывается по ширине
*/
int ImageEditor::getPercentScaleInWidth()
{
	return icePlot()->magnifier()->getPercentScaleInWidth();
}

/*!
  \brief Удалить выделенные области
*/
void ImageEditor::cleanSelectedArea()
{
	icePlot()->picker()->deleteAllAreas();
}

/*!
  \brief Получить все выделенные области
*/
QVariant ImageEditor::getSelectedArea()
{
	return icePlot()->picker()->getSelectedArea();
}

/*!
  \brief Установить режим работы редактора
*/
void ImageEditor::setMode(MODE mode)
{
	d_data->mode = mode;
	if(mode != NOMODE)
	{
		d_data->lastPickerMode = mode;
	}
	switch(mode)
	{
		case PICKER_RECT:
		{
			icePlot()->setMode(ICEQWT_NS::IceQwtPlot::PICKER_RECT);
			break;
		}
		case PICKER_ELLIPSE:
		{
			icePlot()->setMode(ICEQWT_NS::IceQwtPlot::PICKER_ELLIPSE);
			break;
		}
		case PICKER_POLYGON:
		{
			icePlot()->setMode(ICEQWT_NS::IceQwtPlot::PICKER_POLYGON);
			break;
		}
		case PICKER_POLYLINE:
		{
			icePlot()->setMode(ICEQWT_NS::IceQwtPlot::PICKER_POLYLINE);
			break;
		}
		case MOVE:
		{
			icePlot()->setMode(ICEQWT_NS::IceQwtPlot::MOVE);
			break;
		}
		case NOMODE:
		{
			icePlot()->setMode(ICEQWT_NS::IceQwtPlot::NOMODE);
			break;
		}
	}
}

/*!
  \brief Получить режим работы редактора
*/
ImageEditor::MODE ImageEditor::getMode() const
{
	return d_data->mode;
}

/*!
  \brief Получить последний режим работы в режиме выделения области
*/
ImageEditor::MODE ImageEditor::getLastPickerMode() const
{
	return d_data->lastPickerMode;
}

/*!
  \brief Получить последний режим работы в режиме навигации
*/
ImageEditor::MODE ImageEditor::getLastNavigationMode() const
{
	return d_data->lastNavigationMode;
}

} // namespace ICEQWT_NS
