#include "avglumprocplugin.h"

#include <QtCore/QtPlugin>

#include <QtGui/QProgressDialog>

#include <iceodb/odb/filterparam.hxx>

#include <iceimage/imagebuilder.h>

#include "editwidget.h"
#include "avglumproc.h"

#include <opencv/cv.h>

namespace ICEAVGLUMPROC_NS
{

AvgLumProcPlugin::AvgLumProcPlugin() : ICEPROC_NS::ImageProcInterface()
{

}

AvgLumProcPlugin::~AvgLumProcPlugin()
{

}

QSharedPointer<QObject> AvgLumProcPlugin::createParamObject(int /*index*/) const
{
	return QSharedPointer<Param>(new Param());
}

QString AvgLumProcPlugin::getShortName() const
{
	return tr("ФИ");
}

QVector<int> AvgLumProcPlugin::calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam, const QVector<int>& /*indexVector*/,
	double /*divK*/, int /*unit*/)
{
	Q_ASSERT(!imageBuilder.isNull() && filterParam.get());
	emitFilterCalculated(0, getShortName());
	QVector<QSharedPointer<QObject> > paramVector = getParamVector(filterParam);
	QVector<std::tr1::shared_ptr<ICEODB_NS::MaskView> > maskViewVector = getMaskViewVector(
		filterParam);
	Q_ASSERT(paramVector.size() == maskViewVector.size());
	int countSteps = 0;
	for(int i=Param::IMAGE_BASE_MASK_TYPE + 1; i<paramVector.size(); i++)
	{
		Param* param = qobject_cast<Param*>(paramVector[i].data());
		Q_ASSERT(param);
		if(param->isMaskUsed())
		{
			countSteps++;
		}
	}
	emitMaskProgressRangeChanged(0, countSteps);
	countSteps = 0;
	QVector<int> result;
	double baseLum = 0;
	double baseRed = 0;
	double baseGreen = 0;
	double baseBlue = 0;

	for(int i=0; i<paramVector.size(); i++)
	{
		Param* param = qobject_cast<Param*>(paramVector[i].data());
		Q_ASSERT(param);
		if(i == Param::FILTER_BASE_MASK_TYPE)
		{
			continue;
		}
		if(i == Param::IMAGE_BASE_MASK_TYPE)
		{
			baseLum = param->getAvgAbsoluteLum();
			if(baseLum == 0.0)
			{
				baseLum = 765.0;
			}
			baseRed = param->getAvgAbsoluteRed();
			if(baseRed == 0.0)
			{
				baseRed = 255.0;
			}
			baseGreen = param->getAvgAbsoluteGreen();
			if(baseGreen == 0.0)
			{
				baseGreen = 255.0;
			}
			baseBlue = param->getAvgAbsoluteBlue();
			if(baseBlue == 0.0)
			{
				baseBlue = 255.0;
			}
			continue;
		}
		if(!param->isMaskUsed())
		{
			continue;
		}

		int channelTypeValue = param->getChannelType();
		double y = param->getLum();
		double yKMinLum = param->getKMinLum();
		double yKMaxLum = param->getKMaxLum();
		double yK = param->getKLum();

		if(!param->isKLumUsed())
		{
			yK = 1.0;
		}
		double baseValue = baseLum;
		if(channelTypeValue == Param::R_TYPE)
		{
			baseValue = baseRed;
		}
		else if(channelTypeValue == Param::G_TYPE)
		{
			baseValue = baseGreen;
		}
		else if(channelTypeValue == Param::B_TYPE)
		{
			baseValue = baseBlue;
		}
		//double maxLum = baseValue * y * (1 + yK * (yKMaxLum - 1));
		//double minLum = baseValue * y * (1 - yK * (1 - yKMinLum));

		double maxLum = baseValue * y * yKMaxLum;
		double minLum = baseValue * y * yKMinLum;
		double blackLum = 0.0;
		double whiteLum = 765.0;
		if(ICEIMAGE_NS::ImageBuilder::cudaDeviceCount > 0)
		{
			cv::gpu::GpuMat gpuBlackMatMask;
			if(param->isDontUseBlack())
			{
				blackLum = param->getDontUseBlackValue();
				Proc<cv::gpu::GpuMat>::avgLum(imageBuilder->getMatImage<cv::gpu::GpuMat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), gpuBlackMatMask, channelTypeValue,
					blackLum, 0);
			}
			cv::gpu::GpuMat gpuWhiteMatMask;
			if(param->isDontUseWhite())
			{
				whiteLum = param->getDontUseWhiteValue();
				Proc<cv::gpu::GpuMat>::avgLum(imageBuilder->getMatImage<cv::gpu::GpuMat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), gpuWhiteMatMask, channelTypeValue,
					765, whiteLum);
			}
			cv::gpu::GpuMat gpuBlackWhiteMatMask;
			if(!gpuWhiteMatMask.empty() && !gpuBlackMatMask.empty())
			{
				cv::gpu::bitwise_or(gpuWhiteMatMask, gpuBlackMatMask, gpuBlackWhiteMatMask);
			}
			else
			{
				if(!gpuWhiteMatMask.empty())
				{
					gpuBlackWhiteMatMask = gpuWhiteMatMask;
				}
				if(!gpuBlackMatMask.empty())
				{
					gpuBlackWhiteMatMask = gpuBlackMatMask;
				}
			}
			int skippedPointCount = 0;
			if(!gpuBlackWhiteMatMask.empty())
			{
				skippedPointCount = cv::gpu::countNonZero(gpuBlackWhiteMatMask);
			}
			cv::gpu::GpuMat gpuMatMask;
			if(maxLum > whiteLum)
			{
				maxLum = whiteLum;
			}
			if(minLum < blackLum)
			{
				minLum = blackLum;
			}
			Proc<cv::gpu::GpuMat>::avgLum(imageBuilder->getMatImage<cv::gpu::GpuMat>(
				ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), gpuMatMask, channelTypeValue,
				maxLum, minLum);
			imageBuilder->addMask<cv::gpu::GpuMat>(gpuMatMask, maskViewVector[i],
				skippedPointCount);
		}
		else
		{
			cv::Mat blackMatMask;
			if(param->isDontUseBlack())
			{
				blackLum = param->getDontUseBlackValue();
				Proc<cv::Mat>::avgLum(imageBuilder->getMatImage<cv::Mat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), blackMatMask, channelTypeValue,
					blackLum, 0);
			}
			cv::Mat whiteMatMask;
			if(param->isDontUseWhite())
			{
				whiteLum = param->getDontUseWhiteValue();
				Proc<cv::Mat>::avgLum(imageBuilder->getMatImage<cv::Mat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), whiteMatMask, channelTypeValue,
					765, whiteLum);
			}
			cv::Mat blackWhiteMatMask;
			if(!whiteMatMask.empty() && !blackMatMask.empty())
			{
				cv::bitwise_or(whiteMatMask, blackMatMask, blackWhiteMatMask);
			}
			else
			{
				if(!whiteMatMask.empty())
				{
					blackWhiteMatMask = whiteMatMask;
				}
				if(!blackMatMask.empty())
				{
					blackWhiteMatMask = blackMatMask;
				}
			}
			int skippedPointCount = 0;
			if(!blackWhiteMatMask.empty())
			{
				skippedPointCount = cv::countNonZero(blackWhiteMatMask);
			}
			cv::Mat matMask;
			if(maxLum > whiteLum)
			{
				maxLum = whiteLum;
			}
			if(minLum < blackLum)
			{
				minLum = blackLum;
			}
			Proc<cv::Mat>::avgLum(imageBuilder->getMatImage<cv::Mat>(ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE),
				matMask, channelTypeValue, maxLum, minLum);
			imageBuilder->addMask<cv::Mat>(matMask, maskViewVector[i], skippedPointCount);
		}
		result.append(imageBuilder->getMaskListSize() - 1);
		countSteps++;
		emitMaskProgressValueChanged(countSteps);
		emitMaskProgressTextChanged(getShortName());
	}
	imageBuilder->startCalc();
	return result;
}

QSharedPointer<QObject> AvgLumProcPlugin::calcParamObject(int index, QSharedPointer<QObject> param,
	const QImage& image, const QRect& rect) const
{
	cv::Mat matImage = ICEIMAGE_NS::ImageBuilder::fromQImage(image);
	if(matImage.empty())
	{
		return QSharedPointer<Param>();
	}

	double numPixel = 0;
	double bMA = 0;
	double gMA = 0;
	double rMA = 0;
	double lumMA = 0;

	bool dontUseBlack = false;
	bool dontUseWhite = false;
	int dontUseBlackValue = 0;
	int dontUseWhiteValue = 765;
	Param* paramItem = qobject_cast<Param*>(param.data());
	if(paramItem)
	{
		dontUseBlack = paramItem->isDontUseBlack();
		dontUseWhite = paramItem->isDontUseWhite();
		dontUseBlackValue = paramItem->getDontUseBlackValue();
		dontUseWhiteValue = paramItem->getDontUseWhiteValue();
	}

	int lumPixel = 0;
	Q_ASSERT(matImage.type() == CV_8UC4);
	cv::MatConstIterator_<cv::Vec4b> iterEnd = matImage.end<cv::Vec4b>();
	for(cv::MatConstIterator_<cv::Vec4b> iter = matImage.begin<cv::Vec4b>();
		iter != iterEnd; iter++)
	{
		const cv::Vec4b& pixel = *iter;
		if(pixel[3] == 0)
		{
			continue;
		}
		lumPixel = pixel[0] + pixel[1] + pixel[2];
		if(dontUseBlack && lumPixel <= dontUseBlackValue)
		{
			continue;
		}
		if(dontUseWhite && lumPixel >= dontUseWhiteValue)
		{
			continue;
		}
		numPixel++;
	}

	for(cv::MatConstIterator_<cv::Vec4b> iter = matImage.begin<cv::Vec4b>(); iter != iterEnd;
		iter++)
	{
		const cv::Vec4b& pixel = *iter;
		lumPixel = pixel[0] + pixel[1] + pixel[2];
		if(dontUseBlack && lumPixel <= dontUseBlackValue)
		{
			continue;
		}
		if(dontUseWhite && lumPixel >= dontUseWhiteValue)
		{
			continue;
		}
		double lum = lumPixel;
		bMA += pixel[0] / numPixel;
		gMA += pixel[1] / numPixel;
		rMA += pixel[2] / numPixel;
		lumMA += lum / numPixel;
	}
	QSharedPointer<Param> result(new Param());

	lumMA = cvCeil(lumMA);
	rMA = cvCeil(rMA);
	gMA = cvCeil(gMA);
	bMA = cvCeil(bMA);

	result->setAvgAbsoluteLum(lumMA);
	result->setAvgAbsoluteRed(rMA);
	result->setAvgAbsoluteGreen(gMA);
	result->setAvgAbsoluteBlue(bMA);

	if(index == Param::FILTER_BASE_MASK_TYPE || index == Param::IMAGE_BASE_MASK_TYPE)
	{
		if(!rect.isNull())
		{
			result->setBaseImageRect(rect);
		}
		else
		{
			result->setBaseImageRect(QRect(0, 0, image.width(), image.height()));
		}
	}
	if(index >= Param::MASK_TYPE)
	{
		double deltaB = 0;
		double deltaG = 0;
		double deltaR = 0;
		double deltaLum = 0;
		Q_ASSERT(matImage.type() == CV_8UC4);
		for(cv::MatConstIterator_<cv::Vec4b> iter = matImage.begin<cv::Vec4b>();
			iter != iterEnd; iter++)
		{
			const cv::Vec4b& pixel = *iter;
			int lumPixel = pixel[0] + pixel[1] + pixel[2];
			if(dontUseBlack && lumPixel <= dontUseBlackValue)
			{
				continue;
			}
			if(dontUseWhite && lumPixel >= dontUseWhiteValue)
			{
				continue;
			}
			double lum = lumPixel;
			deltaB += abs(pixel[0] - rMA) / numPixel;
			deltaG += abs(pixel[1] - gMA) / numPixel;
			deltaR += abs(pixel[2] - bMA) / numPixel;
			deltaLum += abs(lum - lumMA) / numPixel;
		}
		if(rMA > 0.0)
		{
			deltaR = deltaR / rMA;
		}
		if(gMA > 0.0)
		{
			deltaG = deltaG / gMA;
		}
		if(bMA > 0.0)
		{
			deltaB = deltaB / bMA;
		}
		if(lumMA > 0.0)
		{
			deltaLum = deltaLum / lumMA;
		}
		result->setAvgAbsoluteAutoDeltaLum(deltaLum);
		result->setAvgAbsoluteAutoDeltaRed(deltaR);
		result->setAvgAbsoluteAutoDeltaGreen(deltaG);
		result->setAvgAbsoluteAutoDeltaBlue(deltaB);
	}

	//TODO remove?
	if(index == Param::FILTER_BASE_MASK_TYPE)
	{
		paramItem->setDontUseBlack(dontUseBlack);
		paramItem->setDontUseWhite(dontUseWhite);
		paramItem->setDontUseBlackValue(dontUseBlackValue);
		paramItem->setDontUseWhiteValue(dontUseWhiteValue);
	}

	result->setDontUseBlack(dontUseBlack);
	result->setDontUseWhite(dontUseWhite);
	result->setDontUseBlackValue(dontUseBlackValue);
	result->setDontUseWhiteValue(dontUseWhiteValue);
	return result;
}

QString AvgLumProcPlugin::getViewName(bool severalItems) const
{
	if(severalItems)
	{
		return tr("Фильтры интенсивности");
	}
	else
	{
		return tr("Фильтр интенсивности");
	}
}

QIcon AvgLumProcPlugin::getViewIcon() const
{
	return QIcon(":/dvg/iceavglumproc/images/filterintensity.png");
}

bool AvgLumProcPlugin::prepareFilter(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam)
{
	Q_ASSERT(!imageBuilder.isNull() && filterParam.get());
	QVector<QSharedPointer<QObject> > paramVector;
	for(int i=0; i<filterParam->getMaskParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskParam> maskParam = filterParam->getMaskParam(i);
		paramVector.append(fromJSonString(i, maskParam->getJSonParam()));
	}
	Q_ASSERT(paramVector.size() > Param::IMAGE_BASE_MASK_TYPE);
	Param* filterBaseItem = qobject_cast<Param*>(paramVector[Param::FILTER_BASE_MASK_TYPE].data());
	QImage image;
	QRect rect;
	if(filterBaseItem)
	{
		if(filterBaseItem->isUseRectForBase())
		{
			Param* imageBaseItem = qobject_cast<Param*>(paramVector[Param::IMAGE_BASE_MASK_TYPE].data());
			if(imageBaseItem && !imageBaseItem->getBaseImageRect().isNull())
			{
				rect = imageBaseItem->getBaseImageRect();
				imageBuilder->setUserCreatedObject(rect);
				image = imageBuilder->getUserCreatedObjectQImage();
			}
			else
			{
				return false;
			}
		}
		else
		{
			image = imageBuilder->toQImage(ICEIMAGE_NS::ImageBuilder::RESULT_IMAGE_AND_MASK,
				QRect(), true);
		}
	}
	paramVector[Param::IMAGE_BASE_MASK_TYPE] = calcParamObject(Param::IMAGE_BASE_MASK_TYPE,
		paramVector[Param::FILTER_BASE_MASK_TYPE], image, rect);
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> imageBaseMaskParam =
		filterParam->getMaskParam(Param::IMAGE_BASE_MASK_TYPE);
	Q_ASSERT(imageBaseMaskParam.get());
	imageBaseMaskParam->setJSonParam(toJSonString(paramVector[Param::IMAGE_BASE_MASK_TYPE]));
	//paramVector.append(fromJSonString(i, maskParam->getJSonParam()));
	return true;
}

ICEPROC_NS::EditWidget* AvgLumProcPlugin::createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface);
}

} // namespace ICEAVGLUMPROC_NS

Q_EXPORT_PLUGIN2(avglumprocplugin, ICEAVGLUMPROC_NS::AvgLumProcPlugin);
