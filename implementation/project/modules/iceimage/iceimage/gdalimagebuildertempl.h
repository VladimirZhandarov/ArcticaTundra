#include "gdalimagebuilder.h"

#include <iceodb/odb/maskview.hxx>

#include <gdal_priv.h>

namespace ICEIMAGE_NS
{

template<typename T>
void GdalImageBuilder::applyData(bool onlyVisibleRect)
{
	if(!m_gdalUsed)
	{
		ImageBuilder::applyData<T>(onlyVisibleRect);
		return;
	}
	try
	{
		if(!m_gdalImageVector[SOURCE_IMAGE])
		{
			return;
		}
		bool needRecalc = false;
		T workImage;
		QRect rect;
		int scaleX = 0;
		int scaleY = 0;
		if(onlyVisibleRect)
		{
			//TODO refac
			rect = calcRectForScreen(m_xScaleFactor.getUsedValue(),
				m_yScaleFactor.getUsedValue(), m_scaleType.getUsedValue(),
				m_leftTopPointForScale.getUsedValue(), m_screenSize);
			scaleX = m_screenSize.width();
			scaleY = m_screenSize.height();
			int usedWidth = getImageSizeWidth() * m_xScaleFactor.getUsedValue();
			int usedHeight = getImageSizeHeight() * m_yScaleFactor.getUsedValue();
			if(scaleX > usedWidth)
			{
				scaleX = usedWidth;
			}
			if(scaleY > usedHeight)
			{
				scaleY = usedHeight;
			}

			//TODO fix
//			if(!m_xScaleFactor.isUsed() || !m_yScaleFactor.isUsed() ||
//				!m_scaleType.isUsed() || !m_leftTopPointForScale.isUsed())
			{
				m_imageMutex.lock();
				m_matImageVector[SOURCE_IMAGE] = createMatImageFromGdal(SOURCE_IMAGE,
					rect, QSize(scaleX, scaleY));
				m_imageMutex.unlock();
				workImage = MatFunction<T>::fromMat(m_matImageVector[SOURCE_IMAGE]);

				double factorX = ((double)scaleX) / rect.width();
				double factorY = ((double)scaleY) / rect.height();
				m_leftTopPointInScaled = QPoint(rect.x() * factorX, rect.y() * factorY);

				needRecalc = true;
				m_xScaleFactor.use();
				m_yScaleFactor.use();
				m_scaleType.use();
				m_leftTopPointForScale.use();
			}
/*			else
			{
				if(!m_matForShowImageVector[SOURCE_IMAGE].empty())
				{
					workImage = MatFunction<T>::fromMat(m_matForShowImageVector[SOURCE_IMAGE]);
				}
			}*/
		}

		if(needRecalc || !m_gray.isUsed())
		{
			if(m_gray.getUsedValue())
			{
				T grayImage = applyGray(workImage, m_gray.getUsedValue());
				if(!grayImage.empty())
				{
					m_imageMutex.lock();
					m_matImageVector[GRAY_IMAGE] = MatFunction<T>::toMat(grayImage);
					m_imageMutex.unlock();
					workImage = grayImage;
				}
			}
			needRecalc = true;
			m_gray.use();
		}
		else
		{
			if(!m_matImageVector[GRAY_IMAGE].empty() && m_gray.getUsedValue())
			{
				workImage = MatFunction<T>::fromMat(m_matImageVector[GRAY_IMAGE]);
			}
		}

		if(needRecalc || !m_brightness.isUsed() || !m_contrast.isUsed() || !m_intensity.isUsed() ||
			!m_redBalance.isUsed() || !m_greenBalance.isUsed() || !m_blueBalance.isUsed())
		{
			if(m_brightness.getUsedValue() != 0 || m_contrast.getUsedValue() != 0 ||
				m_intensity.getUsedValue() != 100 || m_redBalance.getUsedValue() != 0 ||
				m_greenBalance.getUsedValue() != 0 || m_blueBalance.getUsedValue() != 0)
			{
				T contrastBrightnessImage = applyBrightnessContrastIntensity(workImage,
					m_brightness.getUsedValue(), m_contrast.getUsedValue(), m_intensity.getUsedValue(),
					m_redBalance.getUsedValue(), m_greenBalance.getUsedValue(), m_blueBalance.getUsedValue());
				if(!contrastBrightnessImage.empty())
				{
					m_imageMutex.lock();
					m_matImageVector[CONTRAST_BRIGHTNESS_IMAGE] = MatFunction<T>::toMat(contrastBrightnessImage);
					m_imageMutex.unlock();
					workImage = contrastBrightnessImage;
				}
			}
			else
			{
				m_imageMutex.lock();
				m_matImageVector[CONTRAST_BRIGHTNESS_IMAGE] = cv::Mat();
				m_imageMutex.unlock();
			}
			needRecalc = true;
			m_brightness.use();
			m_contrast.use();
			m_intensity.use();
			m_redBalance.use();
			m_greenBalance.use();
			m_blueBalance.use();
		}
		else
		{
			if(!m_matImageVector[CONTRAST_BRIGHTNESS_IMAGE].empty())
			{
				workImage = MatFunction<T>::fromMat(m_matImageVector[CONTRAST_BRIGHTNESS_IMAGE]);
			}
		}

		if(onlyVisibleRect)
		{
			//TODO fix
			if(!m_gdalColorMaskImageList.isEmpty())
			{
				int firstIndex = -1;
				for(int i=0; i<m_colorMaskShowList.size(); i++)
				{
					if(m_colorMaskShowList[i])
					{
						firstIndex = i;
						break;
					}
				}
				if(firstIndex >= 0 && firstIndex < m_gdalColorMaskImageList.size())
				{
					Q_ASSERT(m_gdalColorMaskImageList[firstIndex]);
					T colorMask = MatFunction<T>::fromMat(createMatImageFromGdal(
						m_gdalColorMaskImageList[firstIndex], rect, QSize(scaleX, scaleY)));

					for(int i=firstIndex + 1; i<m_gdalColorMaskImageList.size(); i++)
					{
						if(i < m_colorMaskShowList.size() && m_colorMaskShowList[i])
						{
							T colorMask2 = MatFunction<T>::fromMat(createMatImageFromGdal(
								m_gdalColorMaskImageList[i], rect, QSize(scaleX, scaleY)));
							MatFunction<T>::overlayImage(colorMask, colorMask2, colorMask);
						}
					}
					T maskedImage = T(workImage.size(), workImage.type());
					MatFunction<T>::overlayImage(workImage, colorMask, maskedImage);
					workImage = maskedImage;
					needRecalc = true;
				}
				m_imageMutex.lock();
				m_matImageVector[RESULT_IMAGE_AND_MASK] = MatFunction<T>::toMat(workImage);
				m_imageMutex.unlock();
				needRecalc = true; //TODO: refac
			}
		}

		if(!m_contourItemModelList.isEmpty())
		{
			cv::Mat matContourImage = cv::Mat(workImage.size(), workImage.type(), cv::Scalar(0));
			cv::Scalar currentColorCv = cv::Scalar(m_currentContourColor.blue(),
				m_currentContourColor.green(), m_currentContourColor.red(), m_currentContourColor.alpha());
			for(int i=0; i<m_contourItemModelList.size(); i++)
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
						if(i == m_currentContoursIndex && m_contourItemModelList[i]->isCheckedItem(j))
						{
							cv::drawContours(matContourImage, contours, j, currentColorCv,
								CV_FILLED);
						}
						else
						{
							cv::drawContours(matContourImage, contours, j, fillColorCv,
								CV_FILLED);
						}
						cv::drawContours(matContourImage, contours, j, lineColorCv, lineWidth);
					}
				}
			}
			T contouredImage = T(workImage.size(), workImage.type());
			T contourImage = MatFunction<T>::fromMat(matContourImage);
			MatFunction<T>::overlayImage(workImage, contourImage, contouredImage);
			workImage = contouredImage;
			needRecalc = true;
		}

		if(!m_lineItemModelList.isEmpty())
		{
			cv::Mat matLineImage = cv::Mat(workImage.size(), workImage.type(), cv::Scalar(0));
			cv::Scalar currentColorCv = cv::Scalar(m_currentLineColor.blue(),
				m_currentLineColor.green(), m_currentLineColor.red(), m_currentLineColor.alpha());
			for(int i=0; i<m_lineItemModelList.size(); i++)
			{
				Q_ASSERT(m_lineItemModelList[i]);
				if(m_lineItemModelList[i]->isVisible())
				{
					QColor color = m_lineItemModelList[i]->getColor();
					cv::Scalar colorCv = cv::Scalar(color.blue(), color.green(),
						color.red(), 255);
					const std::vector<cv::Vec4i>& lines = m_lineItemModelList[i]->getMatLines();
					for(int j=0; j<lines.size(); j++)
					{
						cv::Vec4i line = lines[j];
						if(!m_lineItemModelList[i]->isVisibleItem(j))
						{
							continue;
						}
						if(i == m_currentLinesIndex && m_lineItemModelList[i]->isCheckedItem(j))
						{
							cv::line(matLineImage, cv::Point(line[0], line[1]),
								cv::Point(line[2], line[3]), currentColorCv);
						}
						cv::line(matLineImage, cv::Point(line[0], line[1]),
							cv::Point(line[2], line[3]), colorCv);
					}
				}
			}
			T linedImage = T(workImage.size(), workImage.type());
			T lineImage = MatFunction<T>::fromMat(matLineImage);
			MatFunction<T>::overlayImage(workImage, lineImage, linedImage);
			workImage = linedImage;
			needRecalc = true;
		}

		if(!m_addedTextList.isEmpty())
		{
			//TODO refac
			int fontFace = cv::FONT_HERSHEY_SIMPLEX;
			double fontScale = 1;
			int thickness = 2;
			int deltaWidth = 30;
			int baseline = 0;
			cv::Mat matWorkImage = MatFunction<T>::toMat(workImage);
			cv::Size sumTextSize = cv::Size(0, 0);
			for(int i=0; i<m_addedTextList.size(); i++)
			{
				std::string text = m_addedTextList[i].toStdString();
				cv::Size textSize = cv::getTextSize(text, fontFace, fontScale,
					thickness, &baseline);
				if(textSize.width > sumTextSize.width)
				{
					sumTextSize.width = textSize.width;
				}
				sumTextSize.height += textSize.height + baseline;
			}
			sumTextSize.width += deltaWidth;
			for(int i=0; i<m_addedTextList.size(); i++)
			{
				cv::Point textPoint((matWorkImage.cols - sumTextSize.width),
					(matWorkImage.rows - sumTextSize.height));
				std::string text = m_addedTextList[i].toStdString();
				cv::putText(matWorkImage, text, textPoint, fontFace, fontScale,
					cv::Scalar::all(255), thickness, 8);
				cv::Size textSize = cv::getTextSize(text, fontFace, fontScale,
					thickness, &baseline);
				sumTextSize.height -= (textSize.height + baseline);
			}
			workImage = MatFunction<T>::fromMat(matWorkImage);
		}

		//TODO make
		/*T resizedImage = resize(srcImage);
		if(!resizedImage.empty())
		{
			needRecalc = true;
			//TODO: resize all image
			m_matImageVector[SOURCE_IMAGE] = MatFunction<T>::toMat(resizedImage);
			m_sourceImageSize = QSize(m_matImageVector[SOURCE_IMAGE].cols, m_matImageVector[SOURCE_IMAGE].rows);
		}
		m_imageMutex.unlock();

		T workImage;
		m_imageMutex.lock();
		cv::Mat matWorkImage = m_matImageVector[COLORED_SOURCE_IMAGE];
		if(matWorkImage.empty())
		{
			matWorkImage = m_matImageVector[SOURCE_IMAGE];
		}
		workImage = MatFunction<T>::fromMat(matWorkImage);
		m_imageMutex.unlock();


		if(needRecalc || !m_rotatedAngleDegree.isUsed() || !m_keepSizeOnRotated.isUsed())
		{
			if(m_rotatedAngleDegree.getUsedValue() != 0.0)
			{
				T resultNoMaskImage = applyRotateCenter(workImage,
					m_rotatedAngleDegree.getUsedValue(), m_keepSizeOnRotated.getUsedValue());
				if(!resultNoMaskImage.empty())
				{
					m_imageMutex.lock();
					m_matImageVector[RESULT_IMAGE_NO_MASK] = MatFunction<T>::toMat(resultNoMaskImage);
					m_imageMutex.unlock();
				}
			}
			else
			{
				m_imageMutex.lock();
				m_matImageVector[RESULT_IMAGE_NO_MASK] = cv::Mat();
				m_imageMutex.unlock();
			}
		}
		else
		{
			m_imageMutex.lock();
			m_matImageVector[RESULT_IMAGE_NO_MASK] = cv::Mat();
			m_imageMutex.unlock();
		}



		if(needRecalc || !m_rotatedAngleDegree.isUsed() || !m_keepSizeOnRotated.isUsed())
		{
			if(m_rotatedAngleDegree.getUsedValue() != 0.0)
			{
				T resultImage = applyRotateCenter(workImage, m_rotatedAngleDegree.getUsedValue(),
					m_keepSizeOnRotated.getUsedValue());
				if(!resultImage.empty())
				{
					m_imageMutex.lock();
					m_matImageVector[RESULT_IMAGE_AND_MASK] = MatFunction<T>::toMat(resultImage);
					m_imageMutex.unlock();
					workImage = resultImage;
				}
			}
			else
			{
				m_imageMutex.lock();
				m_matImageVector[RESULT_IMAGE_AND_MASK] = MatFunction<T>::toMat(workImage);
				m_imageMutex.unlock();
			}
			needRecalc = true;
			m_rotatedAngleDegree.use();
			m_keepSizeOnRotated.use();
		}
		else
		{
			if(!m_matImageVector[RESULT_IMAGE_AND_MASK].empty())
			{
				workImage = MatFunction<T>::fromMat(m_matImageVector[RESULT_IMAGE_AND_MASK]);
			}
		}*/
	}
	catch(const cv::Exception& exc)
	{
		m_imageMutex.unlock();
		emit errorProcessed(QString::fromStdString(exc.what()));
	}
}

template<typename T>
void GdalImageBuilder::addMask(const T& maskImage, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView,
	int skippedPointCount)
{
	//TODO fix
	if(maskView.get())
	{
		QColor color = maskView->getColor();
		Q_ASSERT(!m_matImageVector[SOURCE_IMAGE].empty());
		T colorMaskImage(m_matImageVector[SOURCE_IMAGE].size(), m_matImageVector[SOURCE_IMAGE].type(),
			cv::Scalar(0, 0, 0, 0));
		if(color.isValid() && !maskView->isUseImage())
		{
			colorMaskImage = colorMaskImage.setTo(cv::Scalar(color.blue(), color.green(),
				color.red(), maskView->getAlphaImageChannel()), maskImage);
		}
		else
		{
			T workImage;
			m_imageMutex.lock();
			workImage = MatFunction<T>::fromMat(m_matImageVector[SOURCE_IMAGE]);
			m_imageMutex.unlock();
			workImage.copyTo(colorMaskImage, maskImage);
			MatFunction<T>::setAlpha(colorMaskImage, maskView->getAlphaImageChannel(), maskImage);
		}
		cv::Mat image = MatFunction<T>::toMat(colorMaskImage);
		if(!image.empty())
		{
			m_matColorMaskImageList.append(image);
			m_matMaskImageList.append(MatFunction<T>::toMat(maskImage));
		}
	}
	else
	{
		cv::Mat image = MatFunction<T>::toMat(maskImage);
		if(!image.empty())
		{
			m_matColorMaskImageList.append(image);
			m_matMaskImageList.append(MatFunction<T>::toMat(maskImage));
		}
	}
	std::tr1::shared_ptr<ICEODB_NS::MaskView> resultMaskView;
	if(maskView.get())
	{
		resultMaskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>(
			new ICEODB_NS::MaskView(maskView->getName(), maskView->getColor(),
			maskView->isUseImage(), maskView->getAlphaImageChannel(),
			maskView->getFilterParamNameVector(), skippedPointCount));
	}
	m_colorMaskViewList.append(resultMaskView);
	m_colorMaskShowList.append(true);
	m_contourItemModelList.append(0);
}

} // namespace ICEIMAGE_NS
