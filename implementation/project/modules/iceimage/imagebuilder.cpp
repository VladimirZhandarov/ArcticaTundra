#include "imagebuilder.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QtCore/QFileInfo>

#include <QtGui/QColor>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

#include <iceodb/odb/maskview.hxx>

#include "contouritemmodel.h"
#include "lineitemmodel.h"
#include "imagecutupsettings.h"

namespace ICEIMAGE_NS
{

ImageBuilder::ImageBuilder(QObject* parent) : BaseImageBuilder(parent)
{
	init();
}

ImageBuilder::ImageBuilder(const QString& filename, QObject* parent) : BaseImageBuilder(parent)
{
	init();
	load(filename);
}

ImageBuilder::ImageBuilder(const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView,
	QObject* parent) : BaseImageBuilder(parent), m_backgroundImageMaskView(backgroundImageMaskView)
{
	init();
	load(image);
	setBackgroundImageMaskView(m_backgroundImageMaskView);
}

ImageBuilder::~ImageBuilder()
{
	clear();
}

bool ImageBuilder::load(const QString& filename, bool /*onlyNoGdalImage*/)
{
	bool result = false;
	clear();
	m_filename = filename;

	try
	{
		m_imageMutex.lock();
		m_matImageVector[SOURCE_IMAGE] = cv::imread(m_filename.toStdString().c_str());
		if(!m_matImageVector[SOURCE_IMAGE].empty())
		{
			if(cudaDeviceCount > 0)
			{
				m_matImageVector[SOURCE_IMAGE] = convertToBGRAMatImage<cv::gpu::GpuMat>(
					m_matImageVector[SOURCE_IMAGE]);
			}
			else
			{
				m_matImageVector[SOURCE_IMAGE] = convertToBGRAMatImage<cv::Mat>(
					m_matImageVector[SOURCE_IMAGE]);
			}
			result = true;
			m_sourceImageSize = QSize(m_matImageVector[SOURCE_IMAGE].cols, m_matImageVector[SOURCE_IMAGE].rows);
			m_resultImageSize = QSize();
		}
		else
		{
			m_sourceImageSize = QSize();
			m_resultImageSize = QSize();
		}
		m_imageMutex.unlock();
	}
	catch(const cv::Exception& exc)
	{
		m_imageMutex.unlock();
		emit errorProcessed(QString::fromStdString(exc.what()));
	}
	return result;
}

bool ImageBuilder::load(const QImage& image)
{
	bool result = false;
	clear();

	m_imageMutex.lock();
	m_matImageVector[SOURCE_IMAGE] = fromQImage(image).clone();
	if(!m_matImageVector[SOURCE_IMAGE].empty())
	{
		result = true;
		m_sourceImageSize = QSize(m_matImageVector[SOURCE_IMAGE].cols, m_matImageVector[SOURCE_IMAGE].rows);
		m_resultImageSize = QSize();
	}
	else
	{
		m_sourceImageSize = QSize();
		m_resultImageSize = QSize();
	}
	m_imageMutex.unlock();
	return result;
}

void ImageBuilder::setBackgroundImageMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView)
{
	if(backgroundImageMaskView.get())
	{
		m_backgroundImageMaskView = backgroundImageMaskView;
	}
	else
	{
		m_backgroundImageMaskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(
			new ICEODB_NS::MaskView(tr("Фон"), Qt::black, true));
	}
	Q_ASSERT(m_backgroundImageMaskView.get() && !m_matImageVector[SOURCE_IMAGE].empty());
	if(!m_backgroundImageMaskView->isUseImage())
	{
		QColor color = m_backgroundImageMaskView->getColor();
		m_imageMutex.lock();
		m_matImageVector[COLORED_SOURCE_IMAGE] = cv::Mat(m_matImageVector[SOURCE_IMAGE].rows,
			m_matImageVector[SOURCE_IMAGE].cols, m_matImageVector[SOURCE_IMAGE].type(),
			cv::Scalar(color.blue(), color.green(), color.red(),
			m_backgroundImageMaskView->getAlphaImageChannel()));
		m_imageMutex.unlock();
	}
	else
	{
		//TODO refactor
		if(cudaDeviceCount > 0)
		{
			cv::gpu::GpuMat workImage;
			m_imageMutex.lock();
			workImage.upload(m_matImageVector[SOURCE_IMAGE]);
			m_imageMutex.unlock();
			cv::gpu::bitwise_and(workImage, cv::Scalar(255, 255, 255, 0), workImage);
			cv::gpu::bitwise_or(workImage, cv::Scalar(0, 0, 0, m_backgroundImageMaskView->getAlphaImageChannel()),
				workImage);
			m_imageMutex.lock();
			workImage.download(m_matImageVector[COLORED_SOURCE_IMAGE]);
			m_imageMutex.unlock();
		}
		else
		{
			cv::Mat workImage;
			m_imageMutex.lock();
			workImage = m_matImageVector[SOURCE_IMAGE].clone();
			m_imageMutex.unlock();
			cv::bitwise_and(workImage, cv::Scalar(255, 255, 255, 0), workImage);
			cv::bitwise_or(workImage, cv::Scalar(0, 0, 0, m_backgroundImageMaskView->getAlphaImageChannel()),
				workImage);
			m_imageMutex.lock();
			m_matImageVector[COLORED_SOURCE_IMAGE] = workImage;
			m_imageMutex.unlock();
		}
	}
}

bool ImageBuilder::save(const QString& filename, int quality)
{
	//TODO: выкинуть alpha если её не было
	m_imageMutex.lock();
	cv::Mat image = getWorkImageMat(RESULT_IMAGE_AND_MASK);
	bool result = false;
	if(!image.empty())
	{
		std::vector<int> params;
		QFileInfo fileInfo(filename);
		if(fileInfo.suffix() == "jpg")
		{
			params.push_back(CV_IMWRITE_JPEG_QUALITY);
			params.push_back(quality);
		}
		result = cv::imwrite(filename.toStdString().c_str(), image, params);
	}
	m_imageMutex.unlock();
	return result;
}

bool ImageBuilder::createCutDownImage(const QString& settingsFilename)
{
	ImageCutUpSettings settings(settingsFilename);
	QSize size = settings.getImageSize();
	m_imageMutex.lock();
	m_matImageVector[SOURCE_IMAGE] = cv::Mat(size.height(), size.width(), CV_MAKETYPE(CV_8U, 4),
		cv::Scalar(0));
	m_imageMutex.unlock();
	return true;
}

void ImageBuilder::addCutDownImage(const cv::Mat& matImage, const QRect& rect)
{
	m_imageMutex.lock();
	Q_ASSERT(!matImage.empty());
	cv::Mat bgraMatImage = convertToBGRAMatImage<cv::Mat>(matImage);
	Q_ASSERT(!bgraMatImage.empty());
	Q_ASSERT(!m_matImageVector[SOURCE_IMAGE].empty());
	cv::Mat roi(m_matImageVector[SOURCE_IMAGE], cv::Rect(rect.x(), rect.y(),
		rect.width(), rect.height()));
	Q_ASSERT(!roi.empty());
	bgraMatImage.copyTo(roi);
	m_imageMutex.unlock();
}

void ImageBuilder::flushCutDownImage()
{
	Q_ASSERT(!m_matImageVector[SOURCE_IMAGE].empty());
	m_sourceImageSize = QSize(m_matImageVector[SOURCE_IMAGE].cols, m_matImageVector[SOURCE_IMAGE].rows);
	m_resultImageSize = QSize();
}

const QString& ImageBuilder::getFilename() const
{
	return m_filename;
}

int ImageBuilder::getImageSizeWidth() const
{
	if(m_resultImageSize.isEmpty())
	{
		return BaseImageBuilder::getImageSizeWidth();
	}
	return m_resultImageSize.width();
}

int ImageBuilder::getImageSizeHeight() const
{
	if(m_resultImageSize.isEmpty())
	{
		return BaseImageBuilder::getImageSizeHeight();
	}
	return m_resultImageSize.height();
}

void ImageBuilder::addMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	m_colorMaskViewList.append(maskView);
	m_colorMaskShowList.append(true);
	if(m_colorMaskViewList.size() > m_contourItemModelList.size())
	{
		m_contourItemModelList.append(0);
	}
	m_matColorMaskImageList.append(cv::Mat());
	m_matMaskImageList.append(cv::Mat());
}

void ImageBuilder::flushMask(int /*index*/)
{

}

QString ImageBuilder::getMaskName(int index) const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView;
	if(index >= 0 && index < m_colorMaskViewList.size())
	{
		maskView = m_colorMaskViewList[index];
	}
	if(maskView.get())
	{
		return maskView->getName();
	}
	else
	{
		return tr("Маска ") + QString::number(index);
	}
}

void ImageBuilder::showMask(int index, bool show)
{
	if(index >= 0 && index < m_colorMaskShowList.size())
	{
		m_colorMaskShowList[index] = show;
		startCalc();
	}
}

int ImageBuilder::getMaskListSize() const
{
	return m_matColorMaskImageList.size();
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> ImageBuilder::getColorMaskView(int index) const
{
	if(index >=0 && index < m_colorMaskViewList.size())
	{
		return m_colorMaskViewList[index];
	}
	else
	{
		return std::tr1::shared_ptr<ICEODB_NS::MaskView>();
	}
}

void ImageBuilder::setContours(int index, const std::vector<std::vector<cv::Point> >& contours,
	const cv::Mat& matImage, const QString& filterName, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView,
	const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView, int minSizeValue,
	int minPerimeterValue, double minKFormValue, double divK, int unit)
{
	//TODO clean
	//TODO refac
	if(index >= 0 && index < m_colorMaskViewList.size())
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskView> colorMaskView = m_colorMaskViewList[index];
		if(colorMaskView.get() && maskView.get())
		{
			maskView->setSkippedPointCount(colorMaskView->getSkippedPointCount());
		}
	}
	ContourItemModel* contourItemModel = new ContourItemModel(BaseImageBuilder::cudaDeviceCount > 0,
		index, contours, matImage, filterName, maskView, lineView, minSizeValue, minPerimeterValue,
		minKFormValue, divK, unit, true, this);
	m_contourItemModelList.replace(index, contourItemModel);
	connect(contourItemModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(onCurrentContourDataChanged(const QModelIndex&, const QModelIndex&)));
}

void ImageBuilder::setCurrentContoursIndex(int index)
{
	m_currentContoursIndex = index;
	startCalc();
}

int ImageBuilder::getCurrentContoursIndex() const
{
	return m_currentContoursIndex;
}

void ImageBuilder::setCurrentContourColor(const QColor& color)
{
	m_currentContourColor = color;
	if(!m_matImageVector[SOURCE_IMAGE].empty())
	{
		startCalc();
	}
}

QString ImageBuilder::getContoursName(int index) const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView;
	if(index >= 0 && index < m_contourItemModelList.size())
	{
		if(m_contourItemModelList[index])
		{
			return m_contourItemModelList[index]->getName();
		}
	}
	return tr("");
}

void ImageBuilder::showContours(int index, bool show)
{
	if(index >= 0 && index < m_contourItemModelList.size())
	{
		if(m_contourItemModelList[index])
		{
			m_contourItemModelList[index]->setVisible(show);
			startCalc();
		}
	}
}

int ImageBuilder::getContourItemModelListSize() const
{
	return m_contourItemModelList.size();
}

int ImageBuilder::getNotNullContourItemModelListSize() const
{
	int result = 0;
	for(int i=0; i<m_contourItemModelList.size(); i++)
	{
		if(m_contourItemModelList[i])
		{
			result++;
		}
	}
	return result;
}

ContourItemModel* ImageBuilder::getContourItemModel(int index) const
{
	if(index >= 0 && index < m_contourItemModelList.size())
	{
		return m_contourItemModelList[index];
	}
	else
	{
		return 0;
	}
}

ContourItemModel* ImageBuilder::createEmptyContourItemModel(int matImageCols, int matImageRows)
{
	ContourItemModel* result = new ContourItemModel(m_contourItemModelList.size(),
		matImageCols, matImageRows, this);
	m_contourItemModelList.append(result);
	connect(result, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(onCurrentContourDataChanged(const QModelIndex&, const QModelIndex&)));
	return result;
}

void ImageBuilder::addLines(const std::vector<cv::Vec4i>& lines,
	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
{
	LineItemModel* lineItemModel = new LineItemModel(m_lineItemModelList.size(),
		lines, maskView, true, this);
	m_lineItemModelList.append(lineItemModel);
	connect(lineItemModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(onCurrentLineDataChanged(const QModelIndex&, const QModelIndex&)));
}

void ImageBuilder::setCurrentLinesIndex(int index)
{
	m_currentLinesIndex = index;
	startCalc();
}

void ImageBuilder::setCurrentLineColor(const QColor& color)
{
	m_currentLineColor = color;
	if(!m_matImageVector[SOURCE_IMAGE].empty())
	{
		startCalc();
	}
}

QString ImageBuilder::getLinesName(int index) const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView;
	if(index >= 0 && index < m_lineItemModelList.size())
	{
		Q_ASSERT(m_lineItemModelList[index]);
		return m_lineItemModelList[index]->getName();
	}
	else
	{
		return tr("");
	}
}

void ImageBuilder::showLines(int index, bool show)
{
	if(index >= 0 && index < m_lineItemModelList.size())
	{
		Q_ASSERT(m_lineItemModelList[index]);
		m_lineItemModelList[index]->setVisible(show);
		startCalc();
	}
}

int ImageBuilder::getLineItemModelListSize() const
{
	return m_lineItemModelList.size();
}

LineItemModel* ImageBuilder::getLineItemModel(int index) const
{
	if(index >= 0 && index < m_lineItemModelList.size())
	{
		return m_lineItemModelList[index];
	}
	else
	{
		return 0;
	}
}

//TODO use rect
cv::Mat ImageBuilder::getMatMaskImage(int index, const QRect& /*rect*/)
{
	cv::Mat result;
	try
	{
		if(index >= 0 && index < m_matMaskImageList.size())
		{
			result = MatFunction<cv::Mat>::fromMat(m_matMaskImageList[index]);
		}
	}
	catch(const cv::Exception& exc)
	{
		emit errorProcessed(QString::fromStdString(exc.what()));
	}
	return result;
}

void ImageBuilder::setCursorPosition(const QPoint& pos)
{
	m_cursorPosition = pos;
}

const QPoint& ImageBuilder::getCursorPosition() const
{
	return m_cursorPosition;
}

bool ImageBuilder::setCurrentContourByPos(const QPoint& pos)
{
	for(int i=m_contourItemModelList.size()-1; i>=0; i--)
	{
		if(m_contourItemModelList[i] && m_contourItemModelList[i]->isVisible())
		{
			QColor lineColor = m_contourItemModelList[i]->getLineColor();
			QColor fillColor = m_contourItemModelList[i]->getFillColor();
			int lineWidth = m_contourItemModelList[i]->getLineWidth();
			if(lineWidth <= 0)
			{
				lineWidth = 1;
			}
			cv::Scalar lineColorCv = cv::Scalar(lineColor.blue(), lineColor.green(),
				lineColor.red(), 255);
			cv::Scalar fillColorCv = cv::Scalar(fillColor.blue(), fillColor.green(),
				fillColor.red(), fillColor.alpha());
			const std::vector<std::vector<cv::Point> >& contours = m_contourItemModelList[i]->getMatContours();
			for(int j=0; j<contours.size(); j++)
			{
				if(!m_contourItemModelList[i]->isVisibleItem(j))
				{
					continue;
				}
				if(cv::pointPolygonTest(contours[j], cv::Point2f(pos.x(), pos.y()), false) >= 0.0)
				{
					m_currentContoursIndex = i;
					m_contourItemModelList[i]->setOutsideSelectedIndex(j);
					return true;
				}
			}
		}
	}
	return false;
}

QImage ImageBuilder::getUserCreatedObjectQImage()
{
	if(m_selectedArea.isNull())
	{
		return QImage();
	}
	if(m_selectedArea.type() == QVariant::Rect)
	{
		QRect rect = m_selectedArea.toRect();
		QRect workRect = rect.adjusted(m_leftTopPointInScaled.x(), m_leftTopPointInScaled.y(),
			m_leftTopPointInScaled.x(), m_leftTopPointInScaled.y());
		return toQImage(RESULT_IMAGE_AND_MASK, workRect);
	}
	else if(m_selectedArea.type() == QVariant::List)
	{
		if(!isClosedUserCreatedObject())
		{
			return QImage();
		}
		QRect rect = getUserCreatedObjectRect();
		if(rect.isNull())
		{
			return QImage();
		}
		m_imageMutex.lock();
		cv::Mat matWorkImage = getWorkImageMat(SOURCE_IMAGE);
		m_matRectImageVector[SOURCE_IMAGE] = applySelectRect(matWorkImage,
			rect);
		m_imageMutex.unlock();
		cv::Mat matContourImage = cv::Mat(m_matRectImageVector[SOURCE_IMAGE].rows,
			m_matRectImageVector[SOURCE_IMAGE].cols,
			m_matRectImageVector[SOURCE_IMAGE].type(), cv::Scalar(0, 0, 0, 0));
		std::vector<std::vector<cv::Point> > contours;
		contours.push_back(getUserCreatedObjectContour(rect));
		cv::drawContours(matContourImage, contours, 0, cv::Scalar(255, 255, 255, 255),
			CV_FILLED);
		cv::bitwise_and(matContourImage, m_matRectImageVector[SOURCE_IMAGE],
			matContourImage);
		return toQImage(matContourImage).copy();
	}
	return QImage();
}

void ImageBuilder::setUserCreatedObject(const QVariant& selectedArea)
{
	m_selectedArea = selectedArea;
}

const QVariant& ImageBuilder::getUserCreatedObject() const
{
	return m_selectedArea;
}

QRect ImageBuilder::getUserCreatedObjectRect() const
{
	if(m_selectedArea.isNull())
	{
		return QRect();
	}
	if(m_selectedArea.type() == QVariant::Rect)
	{
		return m_selectedArea.toRect();
	}
	else if(m_selectedArea.type() == QVariant::List)
	{
		QList<QVariant> variantList = m_selectedArea.toList();
		if(variantList.isEmpty())
		{
			return QRect();
		}
		QPoint firstPoint = variantList.first().toPoint();
		double maxX = firstPoint.x();
		double minX = firstPoint.x();
		double maxY = firstPoint.y();
		double minY = firstPoint.y();
		QList<QVariant>::const_iterator iterEnd = variantList.constEnd();
		for(QList<QVariant>::const_iterator iter = variantList.constBegin(); iter != iterEnd;
			iter++)
		{
			QPoint point = (*iter).toPoint();
			if(!point.isNull())
			{
				if(maxX < point.x())
				{
					maxX = point.x();
				}
				if(minX > point.x())
				{
					minX = point.x();
				}
				if(maxY < point.y())
				{
					maxY = point.y();
				}
				if(minY > point.y())
				{
					minY = point.y();
				}
			}
		}
		return QRect(QPoint(minX, minY), QPoint(maxX, maxY));
	}
	return QRect();
}

bool ImageBuilder::isClosedUserCreatedObject() const
{
	if(m_selectedArea.type() == QVariant::Rect)
	{
		return true;
	}
	else if(m_selectedArea.type() == QVariant::List)
	{
		QList<QVariant> variantList = m_selectedArea.toList();
		if(variantList.size() < 4)
		{
			return false;
		}
		return (variantList.first().toPoint() == variantList.last().toPoint());
	}
	return false;
}

QColor ImageBuilder::getPointColor(const QPoint& pos)
{
	return BaseImageBuilder::getPointColor(RESULT_IMAGE_AND_MASK, pos);
}

void ImageBuilder::copyData(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& prevImageBuilder)
{
	if(prevImageBuilder.isNull())
	{
		return;
	}
	for(int i=0; i<prevImageBuilder->getMaskListSize(); i++)
	{
		//TODO ??? int skippedPointCount
		std::tr1::shared_ptr<ICEODB_NS::MaskView> colorMaskView = prevImageBuilder->getColorMaskView(i);
		int skippedPointCount = 0;
		if(colorMaskView.get())
		{
			skippedPointCount = colorMaskView->getSkippedPointCount();
		}
		addMask(prevImageBuilder->getMatMaskImage(i), colorMaskView, skippedPointCount);
	}
	for(int i=0; i<prevImageBuilder->getContourItemModelListSize(); i++)
	{
		ContourItemModel* item = prevImageBuilder->getContourItemModel(i);
		if(item)
		{
			setContours(i, item->getMatContours(), item->getMatImage(), item->getFilterName(),
				item->getMaskView(), item->getLineView(), item->getMinSizeValue(),
				item->getMinPerimeterValue(), item->getMinKFormValue(), item->getDivK(),
				item->getUnit());
		}
	}
	m_filename = prevImageBuilder->getFilename();
}

int ImageBuilder::getUsedFilterCount() const
{
	return getMaskListSize() + getContourItemModelListSize();
}

void ImageBuilder::setAddedTextListToBottomRight(const QStringList& /*addedTextList*/)
{
	//TODO fix
	//m_addedTextList = addedTextList;
	//startCalc();
}

cv::Mat ImageBuilder::createMatCutUpImage(int imageType, const QRect& rect)
{
	cv::Mat result;
	m_imageMutex.lock();
	cv::Mat workImage = getWorkImageMat(imageType);
	if(!rect.isNull())
	{
		result = applySelectRect(workImage, rect);
	}
	m_imageMutex.unlock();
	return result;
}

std::vector<cv::Point> ImageBuilder::getUserCreatedObjectContour(const QRect& rect) const
{
	std::vector<cv::Point> result;
	QList<QVariant> variantList = m_selectedArea.toList();
	QList<QVariant>::const_iterator iterEnd = variantList.constEnd();
	for(QList<QVariant>::const_iterator iter = variantList.constBegin();
		iter != iterEnd; iter++)
	{
		QPoint point = (*iter).toPoint();
		if(!point.isNull())
		{
			result.push_back(cv::Point(point.x() - rect.x(), point.y() - rect.y()));
		}
	}
	return result;
}

void ImageBuilder::applyGpu(bool onlyVisibleRect)
{
	applyData<cv::gpu::GpuMat>(onlyVisibleRect);
}

void ImageBuilder::applyCpu(bool onlyVisibleRect)
{
	applyData<cv::Mat>(onlyVisibleRect);
}

void ImageBuilder::onCurrentContourDataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	startCalc();
}

void ImageBuilder::onCurrentLineDataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	startCalc();
}

void ImageBuilder::init()
{
	m_initMutex.lock();
	m_currentContoursIndex = -1;
	m_currentContourColor = Qt::darkGreen;

	m_currentLinesIndex = -1;
	m_currentLineColor = Qt::darkGreen;

	m_matImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_matRectImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_initMutex.unlock();

	m_cursorPosition = QPoint(-1, -1);
}

void ImageBuilder::clear()
{
	m_initMutex.lock();
	m_resultImageSize = QSize();
	m_matImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_matRectImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);

	m_initMutex.unlock();
}

} // namespace ICEIMAGE_NS
