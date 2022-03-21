#include "core.h"

#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

#include <iceproc/imageprocinterface.h>

#include <iceodb/odb/filterparam.hxx>

#include "filteritemmodel.h"

namespace ICECORE_NS
{

Core* Core::instance = 0;

Core* Core::Instance()
{
	if(!instance)
	{
		instance = new Core();
	}
	return instance;
}

Core::Core(QObject* parent) : QObject(parent), m_filterItemModel(new FilterItemModel(this)),
	m_settings(0)
{
	fillImageProcPluginMap();
}

Core::~Core()
{
	delete m_filterItemModel;
}

FilterItemModel* Core::getFilterItemModel() const
{
	return m_filterItemModel;
}

void Core::setSettings(Settings* settings)
{
	m_settings = settings;
}

Settings* Core::getSettings() const
{
	return m_settings;
}

QStringList Core::getImageProcKey() const
{
	return m_imageProcPluginMap.keys();
}

ICEPROC_NS::ImageProcInterface* Core::getImageProc(const QString& key) const
{
	return m_imageProcPluginMap.value(key, 0);
}

bool Core::saveFilterParam(const QString& filename,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const
{
	if(!filterParam.get())
	{
		return false;
	}
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
	{
		return false;
	}
	QDataStream dataStream(&file);

	//TODO refac
	Q_ASSERT(filterParam.get());
	for(int i=0; i<filterParam->getMaskParamCount(); i++)
	{
		getFilterItemModel()->getMaskParam(filterParam, i);
	}
	for(int i=0; i<filterParam->getChildParamCount(); i++)
	{
		std::tr1::shared_ptr<ICEODB_NS::FilterParam> childFilterParam =
			getFilterItemModel()->getChildFilterParam(filterParam, i);
		if(childFilterParam.get())
		{
			for(int j=0; j<childFilterParam->getMaskParamCount(); j++)
			{
				getFilterItemModel()->getMaskParam(childFilterParam, j);
			}
		}
	}
	filterParamToDataStream(dataStream, filterParam);
	file.flush();
	file.close();
	return true;
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> Core::loadFilterParam(const QString& filename) const
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
	{
		return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
	QDataStream dataStream(&file);
	return filterParamFromDataStream(dataStream);
}

void Core::filterParamToDataStream(QDataStream& dataStream,
	const std::tr1::shared_ptr<ICEODB_NS::FilterParam>& filterParam) const
{
	Q_ASSERT(filterParam.get());
	dataStream << filterParam->getName();
	dataStream << filterParam->getDescription();

	std::tr1::shared_ptr<ICEODB_NS::FilterType> filterType = filterParam->getFilterType();
	Q_ASSERT(filterType.get());
	dataStream << filterType->getDllName();

	maskViewToDataStream(dataStream, filterParam->getBackgroundView());
	int childParamCount = filterParam->getChildParamCount();
	dataStream << childParamCount;
	for(int i=0; i<childParamCount; i++)
	{
		filterParamToDataStream(dataStream, filterParam->getChildParam(i));
	}
	int maskParamCount = filterParam->getMaskParamCount();
	dataStream << maskParamCount;
	for(int i=0; i<maskParamCount; i++)
	{
		maskParamToDataStream(dataStream, filterParam->getMaskParam(i));
	}
}

std::tr1::shared_ptr<ICEODB_NS::FilterParam> Core::filterParamFromDataStream(
	QDataStream& dataStream) const
{
	std::tr1::shared_ptr<ICEODB_NS::FilterParam> result;
	QString name;
	QString description;
	QString dllName;
	dataStream >> name;
	dataStream >> description;
	dataStream >> dllName;
	Q_ASSERT(m_filterItemModel);
	std::tr1::shared_ptr<ICEODB_NS::FilterType> filterType = m_filterItemModel->getFilterType(dllName);
	if(!filterType.get())
	{
		return std::tr1::shared_ptr<ICEODB_NS::FilterParam>();
	}
	result = std::tr1::shared_ptr<ICEODB_NS::FilterParam>(
		new ICEODB_NS::FilterParam(name, description, filterType));
	result->setBackgroundView(maskViewFromDataStream(dataStream));
	int childParamCount = 0;
	dataStream >> childParamCount;
	for(int i=0; i<childParamCount; i++)
	{
		result->addChildParam(filterParamFromDataStream(dataStream));
	}
	int maskParamCount = 0;
	dataStream >> maskParamCount;
	for(int i=0; i<maskParamCount; i++)
	{
		result->addMaskParam(maskParamFromDataStream(dataStream));
	}
	return result;
}

void Core::maskParamToDataStream(QDataStream& dataStream,
	const std::tr1::shared_ptr<ICEODB_NS::MaskParam>& maskParam) const
{
	Q_ASSERT(maskParam.get());
	dataStream << maskParam->getJSonParam();
	std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage = maskParam->getFilterImage();
	if(!filterImage.get())
	{
		dataStream << false;
	}
	else
	{
		dataStream << true;
		dataStream << filterImage->getFilename();
		dataStream << filterImage->getImage();
		dataStream << filterImage->getWidth();
		dataStream << filterImage->getHeight();
		dataStream << filterImage->getWidthStep();
		dataStream << filterImage->getFormat();
		dataStream << filterImage->isMakeSmaller();
	}
	std::tr1::shared_ptr<ICEODB_NS::LineView> lineView = maskParam->getLineView();
	if(!lineView.get())
	{
		dataStream << false;
	}
	else
	{
		dataStream << true;
		maskViewToDataStream(dataStream, lineView);
		dataStream << lineView->getLineWidth();
	}
	std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskParam->getMaskView();
	if(!maskView.get())
	{
		dataStream << false;
	}
	else
	{
		dataStream << true;
		maskViewToDataStream(dataStream, maskView);
	}
}

std::tr1::shared_ptr<ICEODB_NS::MaskParam> Core::maskParamFromDataStream(
	QDataStream& dataStream) const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskParam> result(new ICEODB_NS::MaskParam());
	QString jSonParam;
	dataStream >> jSonParam;
	result->setJSonParam(jSonParam);
	bool filterImageExist = false;
	dataStream >> filterImageExist;
	if(filterImageExist)
	{
		QString filename;
		QByteArray image;
		int width;
		int height;
		int widthStep;
		int format;
		bool makeSmaller;
		dataStream >> filename;
		dataStream >> image;
		dataStream >> width;
		dataStream >> height;
		dataStream >> widthStep;
		dataStream >> format;
		dataStream >> makeSmaller;
		std::tr1::shared_ptr<ICEODB_NS::FilterImage> filterImage(
			new ICEODB_NS::FilterImage(filename, image, width, height, widthStep,
			format, makeSmaller));
		result->setFilterImage(filterImage);
	}
	bool lineViewExist = false;
	dataStream >> lineViewExist;
	if(lineViewExist)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskViewFromDataStream(dataStream);
		if(maskView.get())
		{
			int lineWidth = 0;
			dataStream >> lineWidth;
			std::tr1::shared_ptr<ICEODB_NS::LineView> lineView(
				new ICEODB_NS::LineView(maskView->getName(), maskView->getColor(),
				maskView->isUseImage(), maskView->getAlphaImageChannel(),
				maskView->getFilterParamNameList(), lineWidth));
			result->setLineView(lineView);
		}
	}
	bool maskViewExist = false;
	dataStream >> maskViewExist;
	if(maskViewExist)
	{
		std::tr1::shared_ptr<ICEODB_NS::MaskView> maskView = maskViewFromDataStream(dataStream);
		result->setMaskView(maskView);
	}
	return result;
}

void Core::maskViewToDataStream(QDataStream& dataStream,
	const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView) const
{
	Q_ASSERT(maskView.get());
	dataStream << maskView->getName();
	dataStream << maskView->getColor();
	dataStream << maskView->isUseImage();
	dataStream << maskView->getAlphaImageChannel();
}

std::tr1::shared_ptr<ICEODB_NS::MaskView> Core::maskViewFromDataStream(
	QDataStream& dataStream) const
{
	std::tr1::shared_ptr<ICEODB_NS::MaskView> result;
	QString name;
	QColor color;
	bool useImage = false;
	int alphaImageChannel = 255;
	dataStream >> name;
	dataStream >> color;
	dataStream >> useImage;
	dataStream >> alphaImageChannel;
	result = std::tr1::shared_ptr<ICEODB_NS::MaskView>(new ICEODB_NS::MaskView(
		name, color, useImage, alphaImageChannel));
	return result;
}

void Core::fillImageProcPluginMap()
{
	QDir pluginsDir(qApp->applicationDirPath());
	foreach(QString filename, pluginsDir.entryList(QDir::Files))
	{
		QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(filename));
		ICEPROC_NS::ImageProcInterface* imagePlugin =
			qobject_cast<ICEPROC_NS::ImageProcInterface*>(pluginLoader.instance());
		if(imagePlugin)
		{
			QFileInfo fileInfo(pluginLoader.fileName());
			imagePlugin->setDllName(fileInfo.completeBaseName());
			m_imageProcPluginMap.insert(fileInfo.completeBaseName(), imagePlugin);
		}
	}
	//TODO refac
	QStringList keys = m_imageProcPluginMap.keys();
	for(int i=0; i<keys.count(); i++)
	{
		ICEPROC_NS::ImageProcInterface* imagePlugin = m_imageProcPluginMap.value(keys[i]);
		Q_ASSERT(imagePlugin);
		imagePlugin->init();
	}
}

} // namespace ICECORE_NS
