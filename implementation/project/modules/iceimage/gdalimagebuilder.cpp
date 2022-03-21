#include "gdalimagebuilder.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QtGui/QColor>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QProgressDialog>
#include <QtGui/QMessageBox>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include <iceodb/odb/maskview.hxx>

#include "contouritemmodel.h"
#include "lineitemmodel.h"
#include "imagecutupsettings.h"

#include <gdal_priv.h>

namespace ICEIMAGE_NS
{

QString GdalImageBuilder::homePath = "";
QString GdalImageBuilder::tempPath = "";
#define MEMORY_DIV 6
quint64 GdalImageBuilder::maxImageSize = 0;
QWidget* GdalImageBuilder::mainWindow = 0;

GdalImageBuilder::GdalImageBuilder(QObject* parent) : ImageBuilder(parent),
	m_translateProcess(0), m_translateProgressDialog(0)
{
	init();
}

GdalImageBuilder::GdalImageBuilder(const QString& filename, QObject* parent) :
	ImageBuilder(parent), m_translateProcess(0), m_translateProgressDialog(0)
{
	init();
	load(filename);
}

GdalImageBuilder::~GdalImageBuilder()
{
	clear();
}

void GdalImageBuilder::setHomePath(const QString& arg)
{
	homePath = arg;
	if(!homePath.endsWith("/"))
	{
		homePath += "/";
	}
}

void GdalImageBuilder::setTempPath(const QString& arg)
{
	tempPath = arg;
	if(!tempPath.endsWith("/"))
	{
		tempPath += "/";
	}
}

bool GdalImageBuilder::setMaxImageSize(quint64 arg)
{
	size_t systemMaxImageSize = ICEIMAGE_NS::BaseImageBuilder::queryMemory().first / MEMORY_DIV;
	if(arg < systemMaxImageSize)
	{
		maxImageSize = arg;
		return true;
	}
	else
	{
		return false;
	}
}

quint64 GdalImageBuilder::getMaxImageSize()
{
	return maxImageSize;
}

void GdalImageBuilder::setMainWindow(QWidget* arg)
{
	mainWindow = arg;
}

bool GdalImageBuilder::load(const QString& filename, bool onlyNoGdalImage)
{
	clear();
	m_filename = filename;

	m_imageMutex.lock();
	m_gdalUsed = true;
	m_fileSizeText = "";
	GDALDataset* gdalImage = (GDALDataset*)GDALOpenShared(m_filename.toStdString().c_str(),
		GA_ReadOnly);
	if(gdalImage)
	{
		quint64 x = gdalImage->GetRasterXSize();
		quint64 y = gdalImage->GetRasterYSize();
		if(x * y * maxChannelCount > maxImageSize)
		{
			if(onlyNoGdalImage)
			{
				m_imageMutex.unlock();
				return false;
			}
			QString program = QDir::currentPath() + "/gdal_translate.exe";
			QFileInfo programFileInfo(program);
			if(!programFileInfo.exists())
			{
				QMessageBox::warning(mainWindow, tr("Ошибка распаковки изображения"),
					tr("Не найден исполняемый файл gdal_translate.exe\n") +
					tr("в каталоге установки программы.\n") + 
					tr("Переустановите программу."));
				return false;
			}
			QStringList arguments;
			QFileInfo fileInfo(m_filename);
			m_translatedFilename = tempPath + fileInfo.completeBaseName() +
				QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz") +
				".tif";
			m_filename = m_filename.replace("\\", "/");
			m_translatedFilename = m_translatedFilename.replace("\\", "/");
			arguments << "-b" << "1" << "-b" << "2" << "-b" << "3" << "-b" << "mask" <<
				m_filename << m_translatedFilename;

			Q_ASSERT(m_translateProcess);
			connect(m_translateProcess, SIGNAL(error(QProcess::ProcessError)),
				this, SLOT(translateProcess_error(QProcess::ProcessError)));
			connect(m_translateProcess, SIGNAL(finished(int, QProcess::ExitStatus)), 
				this, SLOT(translateProcess_finished(int, QProcess::ExitStatus)));
			connect(m_translateProcess, SIGNAL(readyReadStandardError()),
				this, SLOT(translateProcess_readyReadStandardError()));
			connect(m_translateProcess, SIGNAL(readyReadStandardOutput()),
				this, SLOT(translateProcess_readyReadStandardOutput()));
			connect(m_translateProcess, SIGNAL(started()),
				this, SLOT(translateProcess_started()));
			connect(m_translateProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
				this, SLOT(translateProcess_stateChanged(QProcess::ProcessState)));

			Q_ASSERT(m_translateProgressDialog);
			connect(this, SIGNAL(translateProcessFinished(const QString&)),
				m_translateProgressDialog, SLOT(setCancelButtonText(const QString&)));
			connect(this, SIGNAL(translateProcessStartTextReaded(const QString&)),
				m_translateProgressDialog, SLOT(setLabelText(const QString&)));
			connect(this, SIGNAL(translateProcessValueChanged(int)),
				m_translateProgressDialog, SLOT(setValue(int)));

			connect(m_translateProgressDialog, SIGNAL(canceled()),
				m_translateProcess, SLOT(kill()));

			m_translateProcess->start(program, arguments);
			m_translateProgressDialog->setAutoClose(true);
			m_translateProgressDialog->setWindowTitle(tr("Распаковка изображения"));
			m_translateProgressDialog->exec();
			m_translateProcess->waitForFinished();
			if(!m_translatedFilename.isEmpty())
			{
				m_gdalImageVector[SOURCE_IMAGE] = (GDALDataset*)GDALOpenShared(
					m_translatedFilename.toStdString().c_str(),
					GA_ReadOnly);
				if(m_gdalImageVector[SOURCE_IMAGE])
				{
					m_imageMutex.unlock();
					return true;
				}
			}
			else
			{
				m_imageMutex.unlock();
				return false;
			}
		}
		else
		{
			GDALClose(gdalImage);
			m_gdalUsed = false;
		}
	}
	else
	{
		m_imageMutex.unlock();
		return false;
	}
	m_imageMutex.unlock();
	if(!m_gdalUsed)
	{
		return ImageBuilder::load(filename, onlyNoGdalImage);
	}
	else
	{
		return true;
	}
}

bool GdalImageBuilder::save(const QString& filename, int quality)
{
	if(!m_gdalUsed)
	{
		return ImageBuilder::save(filename, quality);
	}
	m_imageMutex.lock();
	bool result = false;

	QString program = QDir::currentPath() + "/gdal_translate.exe";
	QFileInfo programFileInfo(program);
	if(!programFileInfo.exists())
	{
		QMessageBox::warning(mainWindow, tr("Ошибка сжатия изображения"),
			tr("Не найден исполняемый файл gdal_translate.exe\n") +
			tr("в каталоге установки программы.\n") + 
			tr("Переустановите программу."));
		return false;
	}
	QString localFilename = filename;
	QStringList arguments;
	QFileInfo fileInfo(localFilename);
	//TODO refac
	//*.jpg" << "*.tif" << "*.png" << "*.bmp"
	if(fileInfo.suffix() == "jpg")
	{
		arguments << "-b" << "1" << "-b" << "2" << "-b" << "3" << "-of" << "JPEG" <<
			"-co" << QString("QUALITY=%1").arg(quality);
	}
	else if(fileInfo.suffix() == "tif")
	{

	}
	else if(fileInfo.suffix() == "png")
	{
		arguments << "-of" << "PNG";
	}
	else if(fileInfo.suffix() == "bmp")
	{
		arguments << "-b" << "1" << "-b" << "2" << "-b" << "3" << "-of" << "BMP";
	}
	else
	{
		return false;
	}
	Q_ASSERT(!m_translatedFilename.isEmpty());
	localFilename = localFilename.replace("\\", "/");
	m_translatedFilename = m_translatedFilename.replace("\\", "/");
	arguments << m_translatedFilename << filename;

	Q_ASSERT(m_translateProcess);
	connect(m_translateProcess, SIGNAL(error(QProcess::ProcessError)),
		this, SLOT(translateProcess_error(QProcess::ProcessError)));
	connect(m_translateProcess, SIGNAL(finished(int, QProcess::ExitStatus)), 
		this, SLOT(translateProcess_finished(int, QProcess::ExitStatus)));
	connect(m_translateProcess, SIGNAL(readyReadStandardError()),
		this, SLOT(translateProcess_readyReadStandardError()));
	connect(m_translateProcess, SIGNAL(readyReadStandardOutput()),
		this, SLOT(translateProcess_readyReadStandardOutput()));
	connect(m_translateProcess, SIGNAL(started()),
		this, SLOT(translateProcess_started()));
	connect(m_translateProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
		this, SLOT(translateProcess_stateChanged(QProcess::ProcessState)));

	Q_ASSERT(m_translateProgressDialog);
	connect(this, SIGNAL(translateProcessFinished(const QString&)),	
		m_translateProgressDialog, SLOT(setCancelButtonText(const QString&)));
	connect(this, SIGNAL(translateProcessStartTextReaded(const QString&)),
		m_translateProgressDialog, SLOT(setLabelText(const QString&)));
	connect(this, SIGNAL(translateProcessValueChanged(int)),
		m_translateProgressDialog, SLOT(setValue(int)));

	connect(m_translateProgressDialog, SIGNAL(canceled()),
		m_translateProcess, SLOT(kill()));

	m_translateProcess->start(program, arguments);
	m_translateProgressDialog->setAutoClose(true);
	m_translateProgressDialog->setWindowTitle(tr("Сжатие изображения"));
	m_translateProgressDialog->exec();
	m_translateProcess->waitForFinished();
	result = true; //TODO ???
	m_imageMutex.unlock();
	return result;
}

bool GdalImageBuilder::createCutDownImage(const QString& settingsFilename)
{
	m_gdalUsed = true; //TODO vvv
	ImageCutUpSettings settings(settingsFilename);
	QSize size = settings.getImageSize();
	m_filename = settings.getImageFilename();
	QFileInfo fileInfo(m_filename);
	quint64 x = size.width();
	quint64 y = size.height();
	if(x * y * maxChannelCount > maxImageSize)
	{
		const char* pszFormat = "GTiff";
		GDALDriver* gdalDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
		if(!gdalDriver)
		{
			return false;
		}
		m_translatedFilename = QDir::toNativeSeparators(tempPath + fileInfo.completeBaseName() +
			"_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz") +
			".tif");

		m_imageMutex.lock();
		m_gdalImageVector[SOURCE_IMAGE] = gdalDriver->Create(m_translatedFilename.toStdString().c_str(),
			size.width(), size.height(), 4, GDT_Byte, 0);
		m_imageMutex.unlock();
		return (m_gdalImageVector[SOURCE_IMAGE] != 0);
	}
	else
	{
		m_gdalUsed = false;
		return ImageBuilder::createCutDownImage(settingsFilename);
	}
}

void GdalImageBuilder::addCutDownImage(const cv::Mat& matImage, const QRect& rect)
{
	if(!m_gdalUsed)
	{
		return ImageBuilder::addCutDownImage(matImage, rect);
	}
	m_imageMutex.lock();
	Q_ASSERT(!matImage.empty());
	GDALDataset* gdalImage = m_gdalImageVector[SOURCE_IMAGE];
	Q_ASSERT(gdalImage);

	cv::Mat bgraMatImage = convertToBGRAMatImage<cv::Mat>(matImage);

	int channelsCount = gdalImage->GetRasterCount();
	std::vector<cv::Mat> matBgraImage;
	MatFunction<cv::Mat>::split(bgraMatImage, matBgraImage);
	if(matBgraImage.size() >= 3)
	{
		cv::Mat tempMat = matBgraImage[0];
		matBgraImage[0] = matBgraImage[2];
		matBgraImage[2] = tempMat;
	}
	for(int i=1; i<=channelsCount; i++) // в GDAL каналы считаются с 1 !!!
	{
		GDALRasterBand* currentChannel = gdalImage->GetRasterBand(i);
		CPLErr err = currentChannel->RasterIO(GF_Write, rect.x(), rect.y(), rect.width(),
			rect.height(), (void*)matBgraImage[i - 1].data, rect.width(), rect.height(),
			GDT_Byte, 0, 0);
		if(err != CE_None)
		{

		}
	}
	m_imageMutex.unlock();
}

void GdalImageBuilder::flushCutDownImage()
{
	if(!m_gdalUsed)
	{
		return ImageBuilder::flushCutDownImage();
	}
	m_imageMutex.lock();
	Q_ASSERT(m_gdalImageVector[SOURCE_IMAGE]);
	m_gdalImageVector[SOURCE_IMAGE]->FlushCache();
	m_imageMutex.unlock();
}

GDALDataset* GdalImageBuilder::getWorkImageGdal(int imageType) const
{
	int startImageType = imageType;
	if(startImageType >= m_gdalImageVector.size())
	{
		startImageType = m_gdalImageVector.size() - 1;
	}
	for(int i = imageType; i>=0; i--)
	{
		if(m_gdalImageVector[i])
		{
			return m_gdalImageVector[i];
		}
	}
	return 0;
}

int GdalImageBuilder::getImageSizeWidth() const
{
	if(!m_gdalUsed)
	{
		return ImageBuilder::getImageSizeWidth();
	}
	if(m_gdalImageVector[SOURCE_IMAGE])
	{
		return m_gdalImageVector[SOURCE_IMAGE]->GetRasterXSize();
	}
	else
	{
		return 0;
	}
}

int GdalImageBuilder::getImageSizeHeight() const
{
	if(!m_gdalUsed)
	{
		return ImageBuilder::getImageSizeHeight();
	}
	if(m_gdalImageVector[SOURCE_IMAGE])
	{
		return m_gdalImageVector[SOURCE_IMAGE]->GetRasterYSize();
	}
	else
	{
		return 0;
	}
}

void GdalImageBuilder::addMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView)
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

void GdalImageBuilder::flushMask(int index)
{
	if(!m_gdalUsed)
	{
		ImageBuilder::flushMask(index);
		return;
	}
	GDALDataset* gdalMaskImage = m_gdalMaskImageList[index];
	GDALDataset* gdalColorMaskImage = m_gdalColorMaskImageList[index];
	Q_ASSERT(gdalMaskImage && gdalColorMaskImage);
	gdalMaskImage->FlushCache();
	gdalColorMaskImage->FlushCache();
}

int GdalImageBuilder::getMaskListSize() const
{
	//TODO fix
	//if(!m_gdalUsed)
	{
		return ImageBuilder::getMaskListSize();
	}
	/*else
	{
		return m_gdalMaskImageList.size();
	}*/
}

cv::Mat GdalImageBuilder::getMatMaskImage(int index, const QRect& rect)
{
	//TODO fix
	if(!m_gdalUsed)
	{
		return ImageBuilder::getMatMaskImage(index, rect);
	}
	cv::Mat result;
	try
	{
		if(index >= 0 && index < m_gdalMaskImageList.size())
		{
			result = MatFunction<cv::Mat>::fromMat(createMatImageFromGdal(m_gdalMaskImageList[index], rect));
		}
	}
	catch(const cv::Exception& exc)
	{
		emit errorProcessed(QString::fromStdString(exc.what()));
	}
	return result;
}

QColor GdalImageBuilder::getPointColor(const QPoint& pos)
{
	if(!m_gdalUsed)
	{
		return BaseImageBuilder::getPointColor(RESULT_IMAGE_AND_MASK, pos);
	}
	if(m_xScaleFactor.getUsedValue() <= 0.0 || m_yScaleFactor.getUsedValue() <= 0.0 ||
		pos.x() < 0 || pos.y() < 0)
	{
		return QColor();
	}
	m_imageMutex.lock();
	GDALDataset* image = getWorkImageGdal(RESULT_IMAGE_AND_MASK);
	QColor result;
	if(image && pos.x() < image->GetRasterXSize() && pos.y() < image->GetRasterYSize())
	{
		cv::Vec4b cvColor;
		for(int i=1; i<=image->GetRasterCount(); i++) // в GDAL каналы считаются с 1 !!!
		{
			uchar matBuffer = 0;
			GDALRasterBand* currentChannel = image->GetRasterBand(i);
			CPLErr err = currentChannel->RasterIO(GF_Read, pos.x(), pos.y(),
				1, 1, &matBuffer, 1, 1, GDT_Byte, 0, 0);
			if(err == CE_None)
			{
				cvColor[i - 1] = matBuffer;
			}
		}
		result = QColor(cvColor[0], cvColor[1], cvColor[2], cvColor[3]);
	}
	m_imageMutex.unlock();
	return result;
}

int GdalImageBuilder::getUsedFilterCount() const
{
	return getMaskListSize() + getContourItemModelListSize();
}

std::vector<cv::Point> GdalImageBuilder::getUserCreatedObjectContour(const QRect& rect) const
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

void GdalImageBuilder::applyGpu(bool onlyVisibleRect)
{
	applyData<cv::gpu::GpuMat>(onlyVisibleRect);
}

void GdalImageBuilder::applyCpu(bool onlyVisibleRect)
{
	applyData<cv::Mat>(onlyVisibleRect);
}

cv::Mat GdalImageBuilder::createMatImageFromGdal(GDALDataset* dataset, const QRect& rect,
	const QSize& resultSize)
{
	Q_ASSERT(dataset);
	cv::Mat result;
	int channelsCount = dataset->GetRasterCount();
	int srcImageWidth = dataset->GetRasterXSize();
	int srcImageHeight = dataset->GetRasterYSize();
	int nXOff = 0;
	int nYOff = 0;
	if(!rect.isNull())
	{
		nXOff = rect.x();
		nYOff = rect.y();
		srcImageWidth = rect.width();
		srcImageHeight = rect.height();
	}
	int dstImageWidth = srcImageWidth;
	int dstImageHeight = srcImageHeight;
	if(!resultSize.isEmpty())
	{
		dstImageWidth = resultSize.width();
		dstImageHeight = resultSize.height();
	}

	std::vector<cv::Mat> matImageVector;
	std::vector<void*> matBufferVector;
	for(int j=1; j<=channelsCount; j++) // в GDAL каналы считаются с 1 !!!
	{
		void* matBuffer = (void*)malloc(dstImageWidth * dstImageHeight * sizeof(uchar));
		matBufferVector.push_back(matBuffer);
		GDALRasterBand* currentChannel = dataset->GetRasterBand(j);
		CPLErr err = currentChannel->RasterIO(GF_Read, nXOff, nYOff,
			srcImageWidth, srcImageHeight, matBuffer, dstImageWidth,
			dstImageHeight, GDT_Byte, 0, 0);
		if(err == CE_None)
		{
			matImageVector.push_back(cv::Mat(dstImageHeight, dstImageWidth,
				CV_8UC1, matBuffer));
		}
	}
	if(matImageVector.size() >= 3)
	{
		cv::Mat tempMat = matImageVector[0];
		matImageVector[0] = matImageVector[2];
		matImageVector[2] = tempMat;
	}
	MatFunction<cv::Mat>::merge(matImageVector, result);
	for(int i=0; i<matBufferVector.size(); i++)
	{
		delete [] matBufferVector[i];
	}
	return result;
}

cv::Mat GdalImageBuilder::createMatCutUpImage(int imageType, const QRect& rect)
{
	if(!m_gdalUsed)
	{
		return ImageBuilder::createMatCutUpImage(imageType, rect);
	}
	return createMatImageFromGdal(imageType, rect);
}

cv::Mat GdalImageBuilder::createMatImageFromGdal(int imageType, const QRect& rect,
	const QSize& resultSize, int* usedIndex)
{
	cv::Mat result;
	for(int i=imageType; i>=0; i--)
	{
		if(m_gdalImageVector[i])
		{
			result = createMatImageFromGdal(m_gdalImageVector[i], rect, resultSize);
			if(usedIndex)
			{
				*usedIndex = i;
			}
			break;
		}
	}
	return result;
}

void GdalImageBuilder::onCurrentContourDataChanged(const QModelIndex& /*topLeft*/,
	const QModelIndex& /*bottomRight*/)
{
	startCalc();
}

void GdalImageBuilder::onCurrentLineDataChanged(const QModelIndex& /*topLeft*/,
	const QModelIndex& /*bottomRight*/)
{
	startCalc();
}

void GdalImageBuilder::translateProcess_error(QProcess::ProcessError /*error*/)
{

}

void GdalImageBuilder::translateProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(exitCode == 0 && exitStatus == QProcess::NormalExit)
	{
		Q_ASSERT(m_translateProgressDialog);
		m_translateProgressDialog->setAutoClose(false);
	}
	else
	{
		m_translatedFilename = "";
	}
}

void GdalImageBuilder::translateProcess_readyReadStandardError()
{
	Q_ASSERT(m_translateProcess);
	QString text = m_translateProcess->readAllStandardError();
	QMessageBox::warning(mainWindow, tr("Ошибка распаковки изображения"), text);
}

void GdalImageBuilder::translateProcess_readyReadStandardOutput()
{
	Q_ASSERT(m_translateProcess);
	QString text = m_translateProcess->readAllStandardOutput();
	int index = text.indexOf("\n");
	if(index != -1 && m_fileSizeText.isEmpty())
	{
		m_fileSizeText = text.left(index);
		text = text.right(text.length() - index);
		emit translateProcessStartTextReaded(m_fileSizeText);
	}

	QStringList textList = text.split(" ");
	if(!textList.isEmpty())
	{
		if(textList.last().isEmpty())
		{
			textList.takeLast();
		}
		if(!textList.isEmpty())
		{
			if(textList.last().indexOf("done") != -1)
			{
				emit translateProcessValueChanged(100);
				emit translateProcessFinished(tr("Закрыть"));
			}
			else
			{
				bool okInt = false;
				int value = textList.last().toInt(&okInt);
				if(okInt)
				{
					emit translateProcessValueChanged(value);
				}
			}
		}
	}
}

void GdalImageBuilder::translateProcess_started()
{

}

void GdalImageBuilder::translateProcess_stateChanged(QProcess::ProcessState /*newState*/)
{

}

void GdalImageBuilder::init()
{
	GDALAllRegister();
	m_initMutex.lock();
	m_gdalImageVector.fill(0, IMAGE_TYPE_SIZE);

	size_t localMaxImageSize = ICEIMAGE_NS::BaseImageBuilder::queryMemory().first / MEMORY_DIV;
	if(maxImageSize == 0 || localMaxImageSize < maxImageSize)
	{
		maxImageSize = (int)localMaxImageSize;
	}

	m_tileSize = QSize(2048, 2048);
	Q_ASSERT(m_translateProcess == 0);
	m_translateProcess = new QProcess(this);
	m_translateProgressDialog;

	Q_ASSERT(m_translateProgressDialog == 0);
	m_translateProgressDialog = new QProgressDialog(tr(""), tr("Отмена"), 0, 100,
		mainWindow);
	m_translateProgressDialog->setMinimumWidth(300);
	m_translateProgressDialog->setWindowModality(Qt::WindowModal);

	m_initMutex.unlock();
}

void GdalImageBuilder::clear()
{
	ImageBuilder::clear();
	m_initMutex.lock();
	//TODO free mem
	m_gdalImageVector.fill(0, IMAGE_TYPE_SIZE);

	m_initMutex.unlock();
}

} // namespace ICEIMAGE_NS
