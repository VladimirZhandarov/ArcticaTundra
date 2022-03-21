#include "cursorimageinfowidget.h"

#include <QtCore/QRectF>

#include <QtGui/QPainter>

#include <iceimage/imagebuilder.h>

#include <iceqwt/imageeditor.h>
#include <iceqwt/iceqwtplot.h>
#include <iceqwt/iceunitratioqwttransform.h>
#include <iceqwt/iceqwtplotrasteritem.h>

#include <qwt_scale_engine.h>

namespace ICEGUI_NS
{

CursorImageInfoWidget::CursorImageInfoWidget(QWidget* parent, Qt::WindowFlags f) :
	QWidget(parent, f), m_imageEditor(0)
{
	ui.setupUi(this);
	ui.filterTreeWidget->setContextMenuEnabled(false);

	ui.filterTreeWidget->hide();
}

CursorImageInfoWidget::~CursorImageInfoWidget()
{

}

void CursorImageInfoWidget::updateState()
{
	//TODO refac слишком много вызовов ->
	if(!m_imageEditor)
	{
		return;
	}
	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)m_imageEditor->icePlot()->
		axisScaleEngine(QwtPlot::xTop)->transformation();

	if(transform->unit() != ICEIMAGE_NS::NONE)
	{
		QString unit;
		QRectF rect = m_imageEditor->icePlot()->rasterItem()->boundingRect();
		transform->unit(unit);
		ui.imageSizeValueUnit->setText(QString::number(rect.width()) + " x " +
			QString::number(rect.height()) + " " + unit);
		ui.imageSizeUnit->show();
		ui.imageSizeValueUnit->show();
	}
	else
	{
		ui.imageSizeUnit->hide();
		ui.imageSizeValueUnit->hide();
	}
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = m_imageEditor->getImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	ui.imageSizeValueL->setText(QString::number(imageBuilder->getImageSizeWidth()) +
		" x " + QString::number(imageBuilder->getImageSizeHeight()));

	onCursorPositionChanged();
	onSelectedAreaChanged();
}

void CursorImageInfoWidget::setImageEditor(const QString& name, const ICEQWT_NS::ImageEditor* imageEditor)
{
	if(imageEditor)
	{
		m_imageEditor = imageEditor;
		ui.nameValueL->setText(name);
		fillFilterParamTreeWidget();
		updateState();
	}
	else
	{
		m_imageEditor = 0;
		ui.imageSizeValueL->setText("0 x 0");
	}
	onCursorPositionChanged();
	onSelectedAreaChanged();
}

void CursorImageInfoWidget::onCursorPositionChanged()
{
	if(!m_imageEditor)
	{
		return;
	}
	QPoint pos = QPoint(-1, -1);
	QColor color;
	int sizeX = 0;
	int sizeY = 0;
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = m_imageEditor->getImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());
	pos = imageBuilder->getCursorPosition();
	color = imageBuilder->getPointColor(pos);
	sizeX = imageBuilder->getImageSizeWidth();
	sizeY = imageBuilder->getImageSizeHeight();

	if(pos.x() >= 0 && pos.x() < sizeX && pos.y() >= 0 && pos.y() < sizeY)
	{
		ICEQWT_NS::IceUnitRatioQwtTransform* transform =
			(ICEQWT_NS::IceUnitRatioQwtTransform*)m_imageEditor->icePlot()->
			axisScaleEngine(QwtPlot::xTop)->transformation();
		QPointF pointScale = transform->invTransform(pos);
		ui.cursorXValueL->setText(QString::number(pointScale.x()));
		ui.cursorYValueL->setText(QString::number(pointScale.y()));
	}
	else
	{
		ui.cursorXValueL->setText("-");
		ui.cursorYValueL->setText("-");
	}
	if(color.isValid())
	{
		ui.rgbValueL->setText(QString::number(color.red()) + ", " +
			QString::number(color.green()) + ", " + QString::number(color.blue()));
		double lum = color.red() + color.green() + color.blue();
		double redF = 1.0 / 3.0;
		double greenF = 1.0 / 3.0;
		double blueF = 1.0 / 3.0;
		if(lum > 0)
		{
			redF = color.red() / lum;
			greenF = color.green() / lum;
			blueF = color.blue() / lum;
		}
		ui.rgb2ValueL->setText(QString::number(redF, 'G', 4) + ", " +
			QString::number(greenF, 'G', 4) + ", " + QString::number(blueF, 'G', 4));
	}
	else
	{
		ui.rgbValueL->setText("-, -, -");
		ui.rgb2ValueL->setText("-, -, -");
	}
}

void CursorImageInfoWidget::onSelectedAreaChanged()
{
	if(!m_imageEditor)
	{
		return;
	}
	QSharedPointer<ICEIMAGE_NS::ImageBuilder> imageBuilder = m_imageEditor->getImageBuilder();
	Q_ASSERT(!imageBuilder.isNull());

	QRect rect;
	QList<QPointF> pointList;
	double argLenght = -1.0;
	double area = -1.0;
	bool closedObject = imageBuilder->isClosedUserCreatedObject(); //TODO refac
	QVariant selectedArea = imageBuilder->getUserCreatedObject();
	if(!selectedArea.isNull())
	{
		if(selectedArea.type() == QVariant::Rect)
		{
			rect = selectedArea.toRect().normalized();
		}
		else if(selectedArea.type() == QVariant::List)
		{
			QList<QVariant> variantList = selectedArea.toList();
			QList<QVariant>::const_iterator iterEnd = variantList.constEnd();
			for(QList<QVariant>::const_iterator iter = variantList.constBegin();
				iter != iterEnd; iter++)
			{
				QPointF point = (*iter).toPoint();
				if(!point.isNull())
				{
					pointList.append(point);
				}
			}
		}
	}
	ICEQWT_NS::IceUnitRatioQwtTransform* transform =
		(ICEQWT_NS::IceUnitRatioQwtTransform*)m_imageEditor->icePlot()->
		axisScaleEngine(QwtPlot::xTop)->transformation();
	Q_ASSERT(transform);

	if(!rect.isNull())
	{
		QRectF rectScale = transform->invTransform(QRectF(rect));
		ui.selectedAreaXValueL->setText(QString::number(rectScale.x()));
		ui.selectedAreaYValueL->setText(QString::number(rectScale.y()));
		ui.selectedAreaWidthValueL->setText(QString::number(rectScale.width()));
		ui.selectedAreaHeightValueL->setText(QString::number(rectScale.height()));
		argLenght = (rectScale.width() + rectScale.height()) * 2;
		area = rectScale.width() * rectScale.height();
	}
	else
	{
		ui.selectedAreaXValueL->setText("-");
		ui.selectedAreaYValueL->setText("-");
		ui.selectedAreaWidthValueL->setText("-");
		ui.selectedAreaHeightValueL->setText("-");
	}

	if(!pointList.isEmpty())
	{
		std::vector<cv::Point> scaledPointList;
		QList<QPointF>::const_iterator iterEnd = pointList.constEnd();
		for(QList<QPointF>::const_iterator iter = pointList.constBegin();
			iter != iterEnd; iter++)
		{
			QPointF scaledPoint = transform->invTransform(*iter);
			if(!scaledPoint.isNull())
			{
				scaledPointList.push_back(cv::Point(scaledPoint.x(), scaledPoint.y()));
			}
		}
		argLenght = cv::arcLength(scaledPointList, closedObject);
		if(closedObject)
		{
			area = cv::contourArea(scaledPointList);
		}
	}

	if(closedObject)
	{
		ui.selectedAreaGB->setTitle(tr("Выделенный участок"));
		ui.selectedArcLengthL->setText(tr("Периметр:"));
		ui.selectedAreaL->setEnabled(true);
		ui.selectedAreaValueL->setEnabled(true);
	}
	else
	{
		ui.selectedAreaGB->setTitle(tr("Измерение расстояния"));
		ui.selectedArcLengthL->setText(tr("Расстояние:"));
		ui.selectedAreaL->setEnabled(false);
		ui.selectedAreaValueL->setEnabled(false);
	}

	QString unit;
	transform->unit(unit);
	ui.selectedAreaGB->setTitle(ui.selectedAreaGB->title() + " (" + unit + ")");

	if(argLenght >= 0.0)
	{
		ui.selectedArcLengthValueL->setText(QString::number(argLenght));
	}
	else
	{
		ui.selectedArcLengthValueL->setText("-");
	}
	if(area >= 0.0)
	{
		ui.selectedAreaValueL->setText(QString::number(area));
	}
	else
	{
		ui.selectedAreaValueL->setText("-");
	}
}

void CursorImageInfoWidget::fillFilterParamTreeWidget()
{

}

} // namespace ICEGUI_NS
