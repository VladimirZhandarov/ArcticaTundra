//==============================================================================
// Название:        Диалоговое окно "О программе"
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: Vladimir Zhandarov $
// Создан:    2014.09.26
// Изменен:   $Date:$
// Модуль:    iceapp
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Имя файла: aboutdialog.cpp
//------------------------------------------------------------------------------
// Примечание: 
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include "aboutdialog.h"

#include <iceimage/baseimagebuilder.h>
#include <iceproc/imageprocinterface.h>

AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
	int deviceCount = ICEIMAGE_NS::BaseImageBuilder::cudaDeviceCount;
	QString text = tr("Количество графических ускорителей в системе - %1").arg(deviceCount);
	for(int i=0; i<deviceCount; i++)
	{
		text += "\r\n" + ICEPROC_NS::ImageProcInterface::getCudaDeviceProperties(i);
		QPair<size_t, size_t> memory = ICEIMAGE_NS::BaseImageBuilder::queryMemory();
		text += "\r\n" + tr("Память GPU всего: ") + ICEIMAGE_NS::BaseImageBuilder::createMemoryString(memory.first) +
			tr(",   свободно: ") + ICEIMAGE_NS::BaseImageBuilder::createMemoryString(memory.second);
	}
	ui.cudaDeviceCountL->setText(text);
}

AboutDialog::~AboutDialog()
{

}

void AboutDialog::setVersion(const QString& version)
{
	ui.versionLbl->setText("Версия: " + version);
}
