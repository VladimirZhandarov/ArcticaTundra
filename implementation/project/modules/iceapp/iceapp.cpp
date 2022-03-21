//==============================================================================
// ��������:        ����������
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
// ��� �����: iceapp.cpp
//------------------------------------------------------------------------------
// ����������: 
//------------------------------------------------------------------------------
// ��������:
//------------------------------------------------------------------------------

#include "iceapp.h"

#include <QtCore/QStringList>

#include <iceimage/imagebuilder.h>
#include <iceimage/gdalimagebuilder.h>

IceApp::IceApp(int& argc, char** argv) : QApplication(argc, argv)
{
	QStringList args = arguments();
	if(args.contains("--no-cuda"))
	{
		ICEIMAGE_NS::ImageBuilder::cudaDeviceCount = 0;
		return;
	}
	int maxImageSizeIndex = args.indexOf("--maxImageSize");
	if(maxImageSizeIndex != -1 && maxImageSizeIndex + 1 < args.size())
	{
		quint64 imageSize = args[maxImageSizeIndex + 1].toULongLong();
		ICEIMAGE_NS::GdalImageBuilder::setMaxImageSize(imageSize);
	}
}

IceApp::~IceApp()
{

}
