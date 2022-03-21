#include "baseimagebuilder.h"

#include "matfunction.h"

#include <QtCore/qmath.h>

namespace ICEIMAGE_NS
{

template<typename T>
cv::Mat BaseImageBuilder::convertToBGRAMatImage(cv::Mat image)
{
	T srcImage = MatFunction<T>::fromMat(image);

	if(srcImage.channels() == 1)
	{
		T result;
		MatFunction<T>::cvtColor(srcImage, result, CV_GRAY2BGRA);
		return MatFunction<T>::toMat(result);
	}
	else if(srcImage.channels() == 3)
	{
		T result;
		MatFunction<T>::cvtColor(srcImage, result, CV_BGR2BGRA);
		return MatFunction<T>::toMat(result);
	}
	return image;
}

template<typename T>
T BaseImageBuilder::resize(const T& srcImage)
{
	if(srcImage.empty())
	{
		return T();
	}
	if(m_xResizeFactor != 1.0 || m_yResizeFactor != 1.0)
	{
		QSize xySize = calcSizeForScaled<T>(srcImage, m_xResizeFactor, m_yResizeFactor,
			m_resizeType);
		if(xySize.isNull())
		{
			return T();
		}
		T result = applyScaleSize<T>(srcImage, xySize, cv::INTER_LINEAR);
		m_xResizeFactor = 1.0;
		m_yResizeFactor = 1.0;
		m_resizeType = SCALE_KEEP_ASPECT;
		return result;
	}
	return T();
}

template<typename T>
QRect BaseImageBuilder::calcRectForScreen(const T& srcImage, double xFactor, double yFactor,
	int type)
{
	if(xFactor <= 0.0 || yFactor <= 0.0 || srcImage.empty())
	{
		return QRect();
	}
	double usedWidth = srcImage.cols * xFactor;
	double usedHeight = srcImage.rows * yFactor;
	if(usedWidth > m_screenSize.width() * scaledMul)
	{
		usedWidth = m_screenSize.width() * scaledMul;
	}
	if(usedHeight > m_screenSize.height() * scaledMul)
	{
		usedHeight = m_screenSize.height() * scaledMul;
	}
	double xSize = usedWidth / xFactor;
	double ySize = usedHeight / yFactor;
	double needXSize = xSize;
	double needYSize = ySize;

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
	int x = m_leftTopPointForScale.getUsedValue().x() - deltaPoint.x();
	if(srcImage.cols - x - needXSize < 0)
	{
		x = srcImage.cols - needXSize;
	}
	int y = m_leftTopPointForScale.getUsedValue().y() - deltaPoint.y();
	if(srcImage.rows - y - needYSize < 0)
	{
		y = srcImage.rows - needYSize;
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
	QRect rect = QRect(QPoint(x, y), QSize(cvCeil(xSize), cvCeil(ySize)));
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
		if(realWidth + realX > srcImage.cols)
		{
			realWidth = srcImage.cols - realX;
		}
		if(realHeight + realY > srcImage.rows)
		{
			realHeight = srcImage.rows - realY;
		}
		return QRect(realX, realY, realWidth, realHeight);
	}
	return QRect();
}

template<typename T>
QSize BaseImageBuilder::calcSizeForScaled(const T& srcImage, double xFactor, double yFactor,
	ScaledType type)
{
	if(xFactor <= 0.0 || yFactor <= 0.0 || srcImage.empty())
	{
		return QSize();
	}
	double xSize = srcImage.cols * xFactor;
	double ySize = srcImage.rows * yFactor;
	if(type == SCALE_TO_HEIGHT)
	{
		xSize = srcImage.cols * yFactor;
		ySize = srcImage.rows * yFactor;
	}
	else if(type == SCALE_TO_WIDTH)
	{
		xSize = srcImage.cols * xFactor;
		ySize = srcImage.rows * xFactor;
	}
	else if(type == SCALE_KEEP_ASPECT)
	{
		double factorMin = xFactor;
		if(factorMin > yFactor)
		{
			factorMin = yFactor;
		}
		xSize = srcImage.cols * factorMin;
		ySize = srcImage.rows * factorMin;
	}
	return QSize(cvCeil(xSize), cvCeil(ySize));
}

template<typename T>
T BaseImageBuilder::applyGray(const T& srcImage, bool gray, ChannelType grayChannel)
{
	if(srcImage.empty())
	{
		return T();
	}
	T tempResult;
	T result;
	if(gray && srcImage.channels() != 1)
	{
		if(grayChannel == RGB_CHANNEL)
		{
			if(srcImage.channels() == 4)
			{
				MatFunction<T>::cvtColor(srcImage, tempResult, CV_RGBA2GRAY);
				MatFunction<T>::cvtColor(tempResult, result, CV_GRAY2BGRA);
			}
			else if(srcImage.channels() == 3)
			{
				MatFunction<T>::cvtColor(srcImage, tempResult, CV_RGB2GRAY);
				MatFunction<T>::cvtColor(tempResult, result, CV_GRAY2RGB);
			}
		}
		else
		{
			if(srcImage.channels() == 3 || srcImage.channels() == 4)
			{
				std::vector<T> bgraImage;
				MatFunction<T>::split(srcImage, bgraImage);
				if(grayChannel == R_CHANNEL)
				{
					bgraImage[0] = bgraImage[2];
					bgraImage[1] = bgraImage[2];
				}
				else if(grayChannel == G_CHANNEL)
				{
					bgraImage[0] = bgraImage[1];
					bgraImage[2] = bgraImage[1];
				}
				else
				{
					bgraImage[1] = bgraImage[0];
					bgraImage[2] = bgraImage[0];
				}
				MatFunction<T>::merge(bgraImage, result);
			}
		}
	}
	return result;
}
	
template<typename T>
T BaseImageBuilder::applyBrightnessContrastIntensity(const T& srcImage, int brightness,
	int contrast, int intensity, int redBalance, int greenBalance, int blueBalance)
{
	brightness = normTo100(brightness);
	contrast = normTo100(contrast);
	redBalance = normTo100(redBalance);
	greenBalance = normTo100(greenBalance);
	blueBalance = normTo100(blueBalance);

	T result;

	uchar lut[256];
	cv::Mat lutMat = cv::Mat(1, 256, CV_8UC1, lut, cv::Mat::CONTINUOUS_FLAG);

	if(srcImage.channels() == 3 || srcImage.channels() == 4)
	{
		std::vector<T> bgraImage;
		MatFunction<T>::split(srcImage, bgraImage);

		if(redBalance == greenBalance && redBalance == blueBalance)
		{
			calcLutMat(lut, brightness, contrast, intensity);
			MatFunction<T>::LUT(bgraImage[0], lutMat, bgraImage[0]);
			MatFunction<T>::LUT(bgraImage[1], lutMat, bgraImage[1]);
			MatFunction<T>::LUT(bgraImage[2], lutMat, bgraImage[2]);
		}
		else
		{
			calcLutMat(lut, brightness + redBalance - ((greenBalance + blueBalance) / 2), contrast, intensity);
			MatFunction<T>::LUT(bgraImage[2], lutMat, bgraImage[2]);
			calcLutMat(lut, brightness + greenBalance - ((redBalance + blueBalance) / 2), contrast, intensity);
			MatFunction<T>::LUT(bgraImage[1], lutMat, bgraImage[1]);
			calcLutMat(lut, brightness + blueBalance - ((greenBalance + redBalance) / 2), contrast, intensity);
			MatFunction<T>::LUT(bgraImage[0], lutMat, bgraImage[0]);
		}
		MatFunction<T>::merge(bgraImage, result);
	}
	else
	{
		T grayImage;
		if(srcImage.channels() == 3)
		{
			MatFunction<T>::cvtColor(srcImage, grayImage, CV_RGB2GRAY);
		}
		else if(srcImage.channels() == 4)
		{
			MatFunction<T>::cvtColor(srcImage, grayImage, CV_RGBA2GRAY);
		}
		else
		{
			grayImage = srcImage;
		}
		calcLutMat(lut, brightness, contrast, intensity);
		MatFunction<T>::LUT(grayImage, lutMat, result);
	}
	return result;
}

template<typename T>
T BaseImageBuilder::applyRotateCenter(const T& srcImage, double angleDegrees, bool keepSizeOnRotated)
{
	if(srcImage.empty() || angleDegrees == 0.0)
	{
		return T();
	}
	double x = ((double)srcImage.cols) / 2;
	double y = ((double)srcImage.rows) / 2;
	double diagonal = qSqrt(x * x + y * y);
	double angleRad = qAsin(x / diagonal) + angleDegrees * CV_PI / 180;
	double xCenter = diagonal * qSin(angleRad);
	double yCenter = diagonal * qCos(angleRad);

	if(keepSizeOnRotated)
	{
		T result = T(srcImage.size(), srcImage.type());
		MatFunction<T>::rotate(srcImage, result, result.size(), angleDegrees, x - xCenter, y - yCenter);
		return result;
	}
	else
	{
		int xSize = srcImage.cols * qCos(angleRad) + srcImage.rows * qSin(angleRad);
		int ySize = srcImage.cols * qSin(angleRad) + srcImage.rows * qCos(angleRad);
		int offsetX = (xSize - srcImage.cols) / 2;
		int offsetY = (ySize - srcImage.rows) / 2;
		T result = T(ySize, xSize, srcImage.type());
		MatFunction<T>::rotate(srcImage, result, result.size(), angleDegrees, x - xCenter + offsetX,
			y - yCenter + offsetY);
		return result;
	}
}

template<typename T>
T BaseImageBuilder::applyScaleSize(const T& srcImage, const QSize& xySize, int interpolation)
{
	if(srcImage.empty() || xySize.isNull())
	{
		return T();
	}
	if(xySize.width() == srcImage.cols && xySize.height() == srcImage.rows)
	{
		return srcImage;
	}
	T result = T(xySize.height(), xySize.width(), srcImage.type());
	MatFunction<T>::resize(srcImage, result, result.size(), 0, 0, interpolation);
	return result;
}

template<typename T>
T BaseImageBuilder::applySelectRect(const T& srcImage, const QRect& rect)
{
	if(srcImage.empty())
	{
		return T();
	}
	QRect workRect = rect;
	if(workRect.x() < 0)
	{
		workRect = workRect.adjusted(-workRect.x(), 0, -workRect.x(), 0);
	}
	if(workRect.y() < 0)
	{
		workRect = workRect.adjusted(0, -workRect.y(), 0, -workRect.y());
	}
	int w = workRect.width();
	int h = workRect.height();
	int matW = workRect.x() + w;
	int matH = workRect.y() + h;

	if(matW > srcImage.cols)
	{
		w = srcImage.cols - workRect.x();
	}
	if(matH > srcImage.rows)
	{
		h = srcImage.rows - workRect.y();
	}
	if(w <= 0 || h <= 0)
	{
		return T();
	}
	return T(srcImage, cv::Rect(workRect.x(), workRect.y(), w, h));
}

template<typename T>
T BaseImageBuilder::getMatImage(int imageType)
{
	try
	{
		return MatFunction<T>::fromMat(getWorkImageMat(imageType));
	}
	catch(const cv::Exception& exc)
	{
		emit errorProcessed(QString::fromStdString(exc.what()));
	}
	return T();
}

} // namespace ICEIMAGE_NS
