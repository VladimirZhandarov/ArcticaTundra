#include "baseimagebuilder.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QtGui/QColor>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

namespace ICEIMAGE_NS
{

int BaseImageBuilder::cudaDeviceCount = cv::gpu::getCudaEnabledDeviceCount();
double BaseImageBuilder::scaledMul = 1.0;
int BaseImageBuilder::maxChannelCount = 4;

BaseImageBuilder::BaseImageBuilder(QObject* parent) : QThread(parent)
{
	init();
}

BaseImageBuilder::~BaseImageBuilder()
{
	//TODO bug fix (fail on application exit) QThread::wait();
	clear();
}

QPair<size_t, size_t> BaseImageBuilder::queryMemory()
{
	size_t totalMem = 0;
	size_t freeMem = 0;
	cv::gpu::DeviceInfo().queryMemory(totalMem, freeMem);
	return QPair<size_t, size_t>(totalMem, freeMem);
}

size_t BaseImageBuilder::calcGpuMemBlockSize(bool forFreeMem, int imageInGpu)
{
	if(imageInGpu <= 0)
	{
		imageInGpu = 1;
	}
	size_t mem = 0;
	if(forFreeMem)
	{
		mem = cv::gpu::DeviceInfo().freeMemory();
	}
	else
	{
		mem = cv::gpu::DeviceInfo().totalMemory();
	}
	mem = mem / (imageInGpu * sizeof(float) * 4);
	mem = qSqrt((double)mem);
	int count = 0;
	while(mem != 0)
	{
		mem = mem >> 1;
		count++;
	}
	return pow((double)2, count);
}

QString BaseImageBuilder::createMemoryString(quint64 memSize)
{
	float memFloat = memSize;
	float nextMemFloat = memFloat / 1024;
	if(nextMemFloat < 0.1)
	{
		return QString::number(memSize) + "á";
	}
	memFloat = nextMemFloat;
	nextMemFloat = memFloat / 1024;
	if(nextMemFloat < 0.1)
	{
		return tr("%1").arg(memFloat, 0, 'G', 2) + "Êá";
	}
	memFloat = nextMemFloat;
	nextMemFloat = memFloat / 1024;
	if(nextMemFloat < 0.1)
	{
		return tr("%1").arg(memFloat, 0, 'G', 2) + "Ìá";
	}
	return tr("%1").arg(nextMemFloat, 0, 'G', 2) + "Ãá";
}

void BaseImageBuilder::setSync(bool sync)
{
	m_sync = sync;
}

bool BaseImageBuilder::isGdalUsed() const
{
	return m_gdalUsed;
}

void BaseImageBuilder::apply(bool makeEmit, bool onlyVisibleRect)
{
	m_initMutex.lock();
	if(cudaDeviceCount > 0)
	{
		applyGpu(onlyVisibleRect);
	}
	else
	{
		applyCpu(onlyVisibleRect);
	}
	m_initMutex.unlock();
	if(makeEmit)
	{
		emit imageReady();
	}
}

void BaseImageBuilder::setBrightness(int brightness)
{
	m_brightness = brightness;
	startCalc();
}

int BaseImageBuilder::getBrightness() const
{
	return m_brightness.getUsedValue();
}

void BaseImageBuilder::setContrast(int contrast)
{
	m_contrast = contrast;
	startCalc();
}

int BaseImageBuilder::getContrast() const
{
	return m_contrast.getUsedValue();
}

void BaseImageBuilder::setIntensity(int intensity)
{
	m_intensity = intensity;
	startCalc();
}

int BaseImageBuilder::getIntensity() const
{
	return m_intensity.getUsedValue();
}

void BaseImageBuilder::setRedBalance(int value)
{
	m_redBalance = value;
	startCalc();
}

int BaseImageBuilder::getRedBalance() const
{
	return m_redBalance.getUsedValue();
}

void BaseImageBuilder::setGreenBalance(int value)
{
	m_greenBalance = value;
	startCalc();
}

int BaseImageBuilder::getGreenBalance() const
{
	return m_greenBalance.getUsedValue();
}

void BaseImageBuilder::setBlueBalance(int value)
{
	m_blueBalance = value;
	startCalc();
}

int BaseImageBuilder::getBlueBalance() const
{
	return m_blueBalance.getUsedValue();
}

void BaseImageBuilder::setGray(bool gray, ChannelType grayChannel)
{
	m_gray = gray;
	m_grayChannel = grayChannel;
	startCalc();
}

bool BaseImageBuilder::isGray() const
{
	return m_gray.getUsedValue();
}

BaseImageBuilder::ChannelType BaseImageBuilder::getGrayChannel() const
{
	return m_grayChannel.getUsedValue();
}

void BaseImageBuilder::setRotatedAngleDegree(double rotatedAngleDegree, bool keepSizeOnRotated)
{
	m_rotatedAngleDegree = rotatedAngleDegree;
	m_keepSizeOnRotated = keepSizeOnRotated;
	startCalc();
}

double BaseImageBuilder::getRotatedAngleDegree() const
{
	return m_rotatedAngleDegree.getUsedValue();
}

bool BaseImageBuilder::isKeepSizeOnRotated() const
{
	return m_keepSizeOnRotated.getUsedValue();
}

void BaseImageBuilder::setScaleFactor(double xScaleFactor, double yScaleFactor,
	ScaledType scaleType)
{
	if(xScaleFactor > 0.0 && yScaleFactor > 0.0)
	{
		m_xScaleFactor = xScaleFactor;
		m_yScaleFactor = yScaleFactor;
		m_scaleType = scaleType;
		startCalc();
	}
}

void BaseImageBuilder::setScaleSize(int xSize, int ySize, ScaledType scaleType)
{
	if(xSize <= 0 || ySize <= 0)
	{
		return;
	}
	if(!m_matImageVector[SOURCE_IMAGE].empty())
	{
		setScaleFactor(((double)xSize) / m_matImageVector[SOURCE_IMAGE].cols,
			((double)ySize) / m_matImageVector[SOURCE_IMAGE].rows, scaleType);
	}
}

double BaseImageBuilder::getScaleFactorX() const
{
	return m_xScaleFactor.getUsedValue();
}

double BaseImageBuilder::getScaleFactorY() const
{
	return m_yScaleFactor.getUsedValue();
}

BaseImageBuilder::ScaledType BaseImageBuilder::getScaleType() const
{
	return m_scaleType.getUsedValue();
}

int BaseImageBuilder::getScaledSizeX() const
{
	if(!m_matImageVector[SOURCE_IMAGE].empty())
	{
		return m_matImageVector[SOURCE_IMAGE].cols * m_xScaleFactor.getUsedValue();
	}
	return 0;
}

int BaseImageBuilder::getScaledSizeY() const
{
	if(!m_matImageVector[SOURCE_IMAGE].empty())
	{
		return m_matImageVector[SOURCE_IMAGE].rows * m_yScaleFactor.getUsedValue();
	}
	return 0;
}

void BaseImageBuilder::setResizeFactor(double xResizeFactor, double yResizeFactor,
	ScaledType resizeType)
{
	if(xResizeFactor > 0.0 && yResizeFactor > 0.0)
	{
		m_xResizeFactor = xResizeFactor;
		m_yResizeFactor = yResizeFactor;
		m_resizeType = resizeType;
		startCalc();
	}
}

void BaseImageBuilder::setScaleRegionRect(const QRect& rect)
{
	bool recalcNeeded = true; //???
	if(m_scaleRegionRect.isNull() || m_scaleRegionRect.x() != rect.x() || m_scaleRegionRect.y() != rect.y())
	{
		recalcNeeded = true;
	}
	m_scaleRegionRect = rect;
	if(recalcNeeded)
	{
		m_leftTopPointForScale = QPoint(m_scaleRegionRect.x(), m_scaleRegionRect.y());
		startCalc();
	}
}

const QRect& BaseImageBuilder::getScaleRegionRect() const 
{
	return m_scaleRegionRect;
}

int BaseImageBuilder::getImageSizeWidth() const
{
	return m_sourceImageSize.width();
}

int BaseImageBuilder::getImageSizeHeight() const
{
	return m_sourceImageSize.height();
}

QRect BaseImageBuilder::adjustedRect(const QRect& rect)
{
	QPoint topLeftPoint = rect.topLeft();
	int dx1 = 0;
	int dy1 = 0;
	if(topLeftPoint.x() < 0)
	{
		dx1 = -topLeftPoint.x();
	}
	if(topLeftPoint.y() < 0)
	{
		dy1 = -topLeftPoint.y();
	}
	QPoint bottomRightPoint = rect.bottomRight();
	int dx2 = 0;
	int dy2 = 0;
	if(bottomRightPoint.x() >= getImageSizeWidth())
	{
		dx2 = getImageSizeWidth() - bottomRightPoint.x() - 1;
	}
	if(bottomRightPoint.y() >= getImageSizeHeight())
	{
		dy2 = getImageSizeHeight() - bottomRightPoint.y() - 1;
	}
	return rect.adjusted(dx1, dy1, dx2, dy2);
}

cv::Mat BaseImageBuilder::getWorkImageMat(int imageType) const
{
	int startImageType = imageType;
	if(startImageType >= m_matImageVector.size())
	{
		startImageType = m_matImageVector.size() - 1;
	}
	for(int i = imageType; i>=0; i--)
	{
		if(!m_matImageVector[i].empty())
		{
			return m_matImageVector[i];
		}
	}
	return cv::Mat();
}

void BaseImageBuilder::run()
{
	apply();
}

QRect BaseImageBuilder::calcRectForScreen(double xFactor, double yFactor, int type,
	const QPoint& leftTopPointForScale, const QSize& screenSize)
{
	int srcImageWidth = getImageSizeWidth();
	int srcImageHeight = getImageSizeHeight();

	if(xFactor <= 0.0 || yFactor <= 0.0 || srcImageWidth <= 0 || srcImageHeight <= 0)
	{
		return QRect();
	}
	int usedWidth = srcImageWidth * xFactor;
	int usedHeight = srcImageHeight * yFactor;
	if(usedWidth > screenSize.width() * scaledMul)
	{
		usedWidth = screenSize.width() * scaledMul;
	}
	if(usedHeight > screenSize.height() * scaledMul)
	{
		usedHeight = screenSize.height() * scaledMul;
	}
	int xSize = usedWidth / xFactor;
	int ySize = usedHeight / yFactor;
	int needXSize = xSize;
	int needYSize = ySize;

	if(type == SCALE_TO_HEIGHT)
	{
		xSize = usedWidth / yFactor;
		ySize = usedHeight / yFactor;
	}
	else if(type == SCALE_TO_WIDTH)
	{
		xSize = usedWidth / xFactor;
		ySize = usedHeight / xFactor;
	}
	else if(type == SCALE_KEEP_ASPECT)
	{
		double factorMin = xFactor;
		if(factorMin > yFactor)
		{
			factorMin = yFactor;
		}
		xSize = usedWidth / factorMin;
		ySize = usedHeight / factorMin;
	}
	QPoint deltaPoint = QPoint((xSize * (scaledMul - 1)) / 2,
		(ySize * (scaledMul - 1)) / 2);
	int x = leftTopPointForScale.x() - deltaPoint.x();
	if(srcImageWidth - x - needXSize < 0)
	{
		x = srcImageWidth - needXSize;
	}
	int y = leftTopPointForScale.y() - deltaPoint.y();
	if(srcImageHeight - y - needYSize < 0)
	{
		y = srcImageHeight - needYSize;
	}
	if(x < 0)
	{
		xSize = xSize + x;
		x = 0;
	}
	else
	{
		xSize = needXSize;
	}
	if(y < 0)
	{
		ySize = ySize + y;
		y = 0;
	}
	else
	{
		ySize = needYSize;
	}
	QRect rect = QRect(QPoint(x, y), QSize(xSize, ySize));
	if(!rect.isNull())
	{
		int realX = rect.x();
		int realY = rect.y();
		int realWidth = rect.width();
		int realHeight = rect.height();
		if(realX < 0)
		{
			realWidth += realX;
			realX = 0;
		}
		if(realY < 0)
		{
			realHeight += realY;
			realY = 0;
		}
		if(realWidth + realX > srcImageWidth)
		{
			realWidth = srcImageWidth - realX;
		}
		if(realHeight + realY > srcImageHeight)
		{
			realHeight = srcImageHeight - realY;
		}
		return QRect(realX, realY, realWidth, realHeight);
	}
	return QRect();
}

void BaseImageBuilder::calcLutMat(uchar* lut, int brightness, int contrast, int intensity)
{
	double brightnessNorm = 127.* brightness / 100;
	double delta = 127.* contrast / 100;
	double a = 255. / (255. - delta * 2);
	double b = a * (brightnessNorm - delta);
	if(contrast < 0)
	{
		delta = -128.*contrast / 100;
		a = (256. - delta * 2) / 255.;
		b = a * brightnessNorm + delta;
	}
	for(int i=0; i<256; i++)
	{
		int v = cvRound(a * i + b) * ((double)intensity) / 100;
		if(v < 0)
		{
			v = 0;
		}
		else if(v > 255)
		{
			v = 255;
		}
		lut[i] = v;
	}
}

int BaseImageBuilder::normTo100(int value)
{
	if(value > 100)
	{
		return 100;
	}
	else if(value < -100)
	{
		return -100;
	}
	return value;
}

QImage BaseImageBuilder::toQImage(const cv::Mat& mat)
{
	if(mat.empty())
	{
		return QImage();
	}
	switch(mat.type())
	{
		case CV_8UC4:
		{
			return QImage(mat.data, mat.cols, mat.rows, (int)mat.step, QImage::Format_ARGB32);
		}
		case CV_8UC3:
		{
			QImage image(mat.data, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB888);
			return image.rgbSwapped();
		}
		case CV_8UC1:
		{
			static QVector<QRgb> sColorTable;
			if(sColorTable.isEmpty())
			{
				for(int i=0; i<256; i++)
				{
					sColorTable.push_back(qRgb(i, i, i));
				}
			}
			QImage image(mat.data, mat.cols, mat.rows, (int)mat.step, QImage::Format_Indexed8);
			image.setColorTable(sColorTable);
			return image;
		}
	};
	return QImage();
}

cv::Mat BaseImageBuilder::fromQImage(const QImage& image)
{
	if(image.isNull())
	{
		return cv::Mat();
	}
	int nChannels = 1;
	switch(image.format())
	{
		case QImage::Format_RGB888:
		{
			nChannels = 3;
			break;
		}
		case QImage::Format_RGB32:
		case QImage::Format_ARGB32:
		{
			nChannels = 4;
			break;
		}
		case QImage::Format_Indexed8:
		{
			nChannels = 1;
			break;
		}
	};
	return cv::Mat(image.height(), image.width(), CV_MAKETYPE(CV_8U, nChannels),
		(void*)image.bits(), image.bytesPerLine());
}

QImage BaseImageBuilder::toQImage(int imageType, const QRect& rect, bool shallowCopy)
{
	QImage result;
	m_imageMutex.lock();
	cv::Mat workImage = getWorkImageMat(imageType);
	if(!rect.isNull())
	{
		QRect workRect = rect.adjusted(-m_leftTopPointInScaled.x(), -m_leftTopPointInScaled.y(),
			-m_leftTopPointInScaled.x(), -m_leftTopPointInScaled.y());
		m_matRectImageVector[imageType] = applySelectRect(workImage, workRect);
		result = toQImage(m_matRectImageVector[imageType]);
	}
	else
	{
		result = toQImage(workImage);
	}
	if(!shallowCopy)
	{
		result = result.copy();
	}
	m_imageMutex.unlock();
	return result;
}

QImage BaseImageBuilder::getScaledImage(const QRect& /*rect*/)
{
	return QImage();
}

QColor BaseImageBuilder::getPointColor(int imageType, const QPoint& pos)
{
	if(m_xScaleFactor.getUsedValue() <= 0.0 || m_yScaleFactor.getUsedValue() <= 0.0 ||
		pos.x() < 0 || pos.y() < 0)
	{
		return QColor();
	}
	m_imageMutex.lock();
	cv::Mat image = getWorkImageMat(imageType);
	QColor result;
	if(!image.empty() && pos.x() < image.cols && pos.y() < image.rows)
	{
		cv::Vec4b cvColor = image.at<cv::Vec4b>(cv::Point(pos.x(), pos.y()));
		result = QColor(cvColor[2], cvColor[1], cvColor[0], cvColor[3]);
	}
	m_imageMutex.unlock();
	return result;
}

void BaseImageBuilder::startCalc()
{
	if(m_sync)
	{
		return;
	}
	if(isRunning())
	{
		m_mutex.lock();
		m_usedParam = false;
		m_mutex.unlock();
	}
	else
	{
		m_mutex.lock();
		m_usedParam = true;
		m_mutex.unlock();
		start();
	}
}

void BaseImageBuilder::onFinished()
{
	if(!m_usedParam)
	{
		m_mutex.lock();
		m_usedParam = true;
		m_mutex.unlock();
		start();
	}
	else
	{
		emit imageReady();
	}
}

//TODO refac init and clear
void BaseImageBuilder::init()
{
	m_initMutex.lock();
	QDesktopWidget* desktopWidget = QApplication::desktop();
	if(desktopWidget)
	{
		m_screenSize = desktopWidget->screenGeometry().size();
	}
	m_leftTopPointForScale = QPoint(0, 0);
	m_leftTopPointInScaled = QPoint(0, 0);
	connect(this, SIGNAL(finished()), this, SLOT(onFinished()));
	m_usedParam = false;

	m_brightness = 0;
	m_contrast = 0;
	m_intensity = 100;
	m_redBalance = 0;
	m_greenBalance = 0;
	m_blueBalance = 0;
	m_gray = false;
	m_grayChannel = RGB_CHANNEL;
	m_rotatedAngleDegree = 0.0;
	m_keepSizeOnRotated = true;
	m_xScaleFactor = 1.0;
	m_yScaleFactor = 1.0;
	m_scaleType = SCALE_KEEP_ASPECT;
	m_xResizeFactor = 1.0;
	m_yResizeFactor = 1.0;
	m_resizeType = SCALE_KEEP_ASPECT;
	m_sync = false;
	m_gdalUsed = false;

	m_matImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_matRectImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_initMutex.unlock();

	if(cudaDeviceCount > 0)
	{
		cv::Mat mat(1, 1, CV_32SC1);
		cv::gpu::GpuMat gpuMat;
		gpuMat.upload(mat);
	}
}

void BaseImageBuilder::clear()
{
	m_initMutex.lock();
	m_sourceImageSize = QSize();
	m_matImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_matRectImageVector.fill(cv::Mat(), IMAGE_TYPE_SIZE);
	m_initMutex.unlock();
}

} // namespace ICEIMAGE_NS
