//==============================================================================
// ��������:        ���������� ���� "� ���������"
// ��������� �����: ��� "���" DeveloperGroup (�. �����-���������)
//==============================================================================
//
// �����:     $Author: Vladimir Zhandarov $
// ������:    2014.09.26
// �������:   $Date:$
// ������:    iceapp
// ���������: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// ��� �����: aboutdialog.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "aboutdialog.h"

#include <iceimage/baseimagebuilder.h>
#include <iceproc/imageprocinterface.h>

AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
	int deviceCount = ICEIMAGE_NS::BaseImageBuilder::cudaDeviceCount;
	QString text = tr("���������� ����������� ����������� � ������� - %1").arg(deviceCount);
	for(int i=0; i<deviceCount; i++)
	{
		text += "\r\n" + ICEPROC_NS::ImageProcInterface::getCudaDeviceProperties(i);
		QPair<size_t, size_t> memory = ICEIMAGE_NS::BaseImageBuilder::queryMemory();
		text += "\r\n" + tr("������ GPU �����: ") + ICEIMAGE_NS::BaseImageBuilder::createMemoryString(memory.first) +
			tr(",   ��������: ") + ICEIMAGE_NS::BaseImageBuilder::createMemoryString(memory.second);
	}
	ui.cudaDeviceCountL->setText(text);
}

AboutDialog::~AboutDialog()
{

}

void AboutDialog::setVersion(const QString& version)
{
	ui.versionLbl->setText("������: " + version);
}
