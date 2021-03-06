#include "avgcolorprocplugin.h"

#include <QtCore/QtPlugin>

#include <iceimage/imagebuilder.h>

#include "editwidget.h"
#include "avgcolorproc.h"
#include "param.h"

#include <opencv/cv.h>

namespace ICEAVGCOLORPROC_NS
{

AvgColorProcPlugin::AvgColorProcPlugin() : ICEPROC_NS::ImageProcInterface()
{

}

AvgColorProcPlugin::~AvgColorProcPlugin()
{

}

QSharedPointer<QObject> AvgColorProcPlugin::createParamObject(int /*index*/) const
{
	return QSharedPointer<Param>(new Param());
}

QString AvgColorProcPlugin::getShortName() const
{
	return tr("ФС");
}

QVector<int> AvgColorProcPlugin::calcMaskVector(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& imageBuilder,
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
	for(int i=0; i<paramVector.size(); i++)
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
	for(int i=0; i<paramVector.size(); i++)
	{
		Param* param = qobject_cast<Param*>(paramVector[i].data());
		Q_ASSERT(param);
		if(!param->isMaskUsed())
		{
			continue;
		}
		cv::Scalar colorF = cv::Scalar(param->getAvgRelativeRed(), param->getAvgRelativeGreen(),
			param->getAvgRelativeBlue());
		cv::Scalar plusF = cv::Scalar(param->getDeltaPlusRelativeRed(), param->getDeltaPlusRelativeGreen(),
			param->getDeltaPlusRelativeBlue());
		cv::Scalar minusF = cv::Scalar(param->getDeltaMinusRelativeRed(), param->getDeltaMinusRelativeGreen(),
			param->getDeltaMinusRelativeBlue());
		double blackLum = 0.0;
		double whiteLum = 765.0;
		if(ICEIMAGE_NS::ImageBuilder::cudaDeviceCount > 0)
		{
			cv::gpu::GpuMat gpuBlackMatMask;
			if(param->isDontUseBlack())
			{
				blackLum = param->getDontUseBlackValue();
				Proc<cv::gpu::GpuMat>::avgLum(imageBuilder->getMatImage<cv::gpu::GpuMat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), gpuBlackMatMask,
					blackLum, 0);
			}
			cv::gpu::GpuMat gpuWhiteMatMask;
			if(param->isDontUseWhite())
			{
				whiteLum = param->getDontUseWhiteValue();
				Proc<cv::gpu::GpuMat>::avgLum(imageBuilder->getMatImage<cv::gpu::GpuMat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), gpuWhiteMatMask,
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
			Proc<cv::gpu::GpuMat>::avgColor(imageBuilder->getMatImage<cv::gpu::GpuMat>(
				ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), gpuMatMask, colorF,
				plusF, minusF, whiteLum, blackLum);
			imageBuilder->addMask<cv::gpu::GpuMat>(gpuMatMask, maskViewVector[i]);
		}
		else
		{
			cv::Mat blackMatMask;
			if(param->isDontUseBlack())
			{
				blackLum = param->getDontUseBlackValue();
				Proc<cv::Mat>::avgLum(imageBuilder->getMatImage<cv::Mat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), blackMatMask,
					blackLum, 0);
			}
			cv::Mat whiteMatMask;
			if(param->isDontUseWhite())
			{
				whiteLum = param->getDontUseWhiteValue();
				Proc<cv::Mat>::avgLum(imageBuilder->getMatImage<cv::Mat>(
					ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), whiteMatMask,
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
			Proc<cv::Mat>::avgColor(imageBuilder->getMatImage<cv::Mat>(
				ICEIMAGE_NS::ImageBuilder::SOURCE_IMAGE), matMask, colorF,
				plusF, minusF, whiteLum, blackLum);
			imageBuilder->addMask<cv::Mat>(matMask, maskViewVector[i]);
		}
		result.append(imageBuilder->getMaskListSize() - 1);
		countSteps++;
		emitMaskProgressValueChanged(countSteps);
		emitMaskProgressTextChanged(getShortName());
	}
	imageBuilder->startCalc();
	return result;
}

QSharedPointer<QObject> AvgColorProcPlugin::calcParamObject(int /*index*/, QSharedPointer<QObject> param,
	const QImage& image, const QRect& /*rect*/) const
{
	cv::Mat matImage = ICEIMAGE_NS::ImageBuilder::fromQImage(image);
	if(matImage.empty())
	{
		return QSharedPointer<Param>();
	}

	double numPixel = 0;
	double bMC, gMC, rMC; bMC = gMC = rMC = 0;
	double bMA, gMA, rMA; bMA = gMA = rMA = 0;

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

	Q_ASSERT(matImage.type() == CV_8UC4);
	cv::MatConstIterator_<cv::Vec4b> iter = matImage.begin<cv::Vec4b>();
	cv::MatConstIterator_<cv::Vec4b> iterEnd = matImage.end<cv::Vec4b>();
	for(; iter != iterEnd; iter++)
	{
		const cv::Vec4b& pixel = *iter;
		double lumPixel = pixel[0] + pixel[1] + pixel[2];
		double bC, gC, rC; bC = gC = rC = 0;

		if(pixel[3] == 0)
		{
			continue;
		}
		if(dontUseBlack && lumPixel <= dontUseBlackValue)
		{
			continue;
		}
		if(dontUseWhite && lumPixel >= dontUseWhiteValue)
		{
			continue;
		}
		if(lumPixel == 0.0)
		{
			bC = gC = rC = 1.0/3.0;
		}
		else
		{
			bC = (pixel[0] == 0.0) ? 0.001 : pixel[0] / lumPixel;
			gC = (pixel[1] == 0.0) ? 0.001 : pixel[1] / lumPixel;
			rC = (pixel[2] == 0.0) ? 0.001 : pixel[2] / lumPixel;
		}
		numPixel++;
		bMC += bC;
		gMC += gC;
		rMC += rC;

		bMA += pixel[0];
		gMA += pixel[1];
		rMA += pixel[2];
	}
	bMC = bMC / numPixel;
	gMC = gMC / numPixel;
	rMC = rMC / numPixel;

	bMA = bMA / numPixel;
	gMA = gMA / numPixel;
	rMA = rMA / numPixel;

	double deltaB = 0;
	double deltaG = 0;
	double deltaR = 0;

	for(iter = matImage.begin<cv::Vec4b>(); iter != iterEnd; iter++)
	{
		const cv::Vec4b& pixel = *iter;
		double lumPixel = pixel[0] + pixel[1] + pixel[2];
		double bC, gC, rC; bC = gC = rC = 0;

		if(pixel[3] == 0)
		{
			continue;
		}
		if(dontUseBlack && lumPixel <= dontUseBlackValue)
		{
			continue;
		}
		if(dontUseWhite && lumPixel >= dontUseWhiteValue)
		{
			continue;
		}
		if(lumPixel == 0.0)
		{
			bC = gC = rC = 1.0/3.0;
		}
		else
		{
			bC = (pixel[0] == 0.0) ? 0.001 : pixel[0] / lumPixel;
			gC = (pixel[1] == 0.0) ? 0.001 : pixel[1] / lumPixel;
			rC = (pixel[2] == 0.0) ? 0.001 : pixel[2] / lumPixel;
		}
		deltaB += abs(bC - bMC) / numPixel;
		deltaG += abs(gC - gMC) / numPixel;
		deltaR += abs(rC - rMC) / numPixel;
	}

	QSharedPointer<Param> result(new Param());
	result->setAvgRelativeBlue(bMC);
	result->setAvgRelativeGreen(gMC);
	result->setAvgRelativeRed(rMC);

	result->setAvgAbsoluteBlue(bMA);
	result->setAvgAbsoluteGreen(gMA);
	result->setAvgAbsoluteRed(rMA);

	result->setAvgAbsoluteAutoDeltaBlue(deltaB);
	result->setAvgAbsoluteAutoDeltaGreen(deltaG);
	result->setAvgAbsoluteAutoDeltaRed(deltaR);

	result->setDontUseBlack(dontUseBlack);
	result->setDontUseBlackValue(dontUseBlackValue);
	result->setDontUseWhite(dontUseWhite);
	result->setDontUseWhiteValue(dontUseWhiteValue);

	return result;
}

QString AvgColorProcPlugin::getViewName(bool severalItems) const
{
	if(severalItems)
	{
		return tr("Фильтры спектральные");
	}
	else
	{
		return tr("Фильтр спектральный");
	}
}

QIcon AvgColorProcPlugin::getViewIcon() const
{
	return QIcon(":/dvg/iceavgcolorproc/images/filtercolor.png");
}

ICEPROC_NS::EditWidget* AvgColorProcPlugin::createEditWidget(ICEPROC_NS::ImageWindowInterface* imageWindowInterface) const
{
	return new EditWidget(this, imageWindowInterface);
}

} // namespace ICEAVGCOLORPROC_NS

Q_EXPORT_PLUGIN2(avgcolorprocplugin, ICEAVGCOLORPROC_NS::AvgColorProcPlugin);
