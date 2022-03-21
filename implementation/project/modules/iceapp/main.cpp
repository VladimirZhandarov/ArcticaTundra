//==============================================================================
// Название:        main
// Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
//==============================================================================
//
// Автор:     $Author: Vladimir Zhandarov$
// Создан:    2014.09.26
// Изменен:   $Date:$
// Модуль:    iceapp
// Платформа: Intel Windows 7 Professional SP1
//
//------------------------------------------------------------------------------
// Примечание:
//------------------------------------------------------------------------------
// Внимание:
//------------------------------------------------------------------------------

#include <QtCore/QTextCodec>
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTranslator>

#include <QtGui/QSplashScreen>

#include <icecore/core.h>
#include <icecore/settings.h>

#include <iceproc/imageprocinterface.h>
#include <iceimage/gdalimagebuilder.h>

#include "iceapp.h"
#include "mainwindow.h"

void clearDir(const QString& dirName)
{
	QDir dir(dirName);
	if(!dir.exists())
	{
		return;
	}
	QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
		QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
	Q_FOREACH(QFileInfo info, list)
	{
		if(info.isDir())
		{
			clearDir(info.absoluteFilePath());
		}
		else
		{
			QFile::remove(info.absoluteFilePath());
		}
	}
}

int main(int argc, char *argv[])
{
	IceApp app(argc, argv);
	QString stringGdalTest = "GDAL_FILENAME_IS_UTF8=";

	QStringList systemEnvironmentList = QProcess::systemEnvironment();
	bool gdalUtf8Finded = false;
	for(int i=0;i<systemEnvironmentList.size();i++)
	{
		if(systemEnvironmentList[i].startsWith(stringGdalTest))
		{
			gdalUtf8Finded = true;
			break;
		}
	}
	if(!gdalUtf8Finded)
	{
		QProcess process;
		QStringList env = QProcess::systemEnvironment();
		env << stringGdalTest + QString("NO");
		process.setEnvironment(env);
		QStringList args = app.arguments();
		Q_ASSERT(!args.isEmpty());
		QString programm = args.takeFirst();
		process.start(programm, args);
		process.waitForFinished(-1);
		return process.exitCode();
	}

	QTranslator qtTranslator;
	if(qtTranslator.load(":/dvg/iceapp/translations/qt_ru.qm"))
	{
		app.installTranslator(&qtTranslator);
	}
	QSplashScreen* splash = new QSplashScreen(QPixmap(":/dvg/iceapp/images/splash.png"));
	splash->show();
	app.processEvents();

	QTimer::singleShot(3000, splash, SLOT(close()));

	QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
	setlocale(LC_ALL, "russian");

	ICECORE_NS::Core* core = ICECORE_NS::Core::Instance();
	QDir homeDir(QDir::toNativeSeparators(QDir::homePath() + homeAppTundra));
 	if(!homeDir.exists())
	{
		homeDir.mkpath(homeDir.absolutePath());
	}
	QDir tempDir(QDir::toNativeSeparators(QDir::homePath() + tempAppIce));
	if(!tempDir.exists())
	{
		tempDir.mkpath(tempDir.absolutePath());
	}
	clearDir(tempDir.absolutePath());
	QFileInfo fileSettings(homeDir, iniSettings);
	ICEIMAGE_NS::GdalImageBuilder::setHomePath(homeDir.absolutePath());
	ICEIMAGE_NS::GdalImageBuilder::setTempPath(tempDir.absolutePath());
	ICECORE_NS::Settings* settings = new ICECORE_NS::Settings(fileSettings);
	core->setSettings(settings);
	MainWindow mainWindow;
	ICEIMAGE_NS::GdalImageBuilder::setMainWindow(&mainWindow);
	QTimer::singleShot(3000, &mainWindow, SLOT(show()));

	app.exec();

	delete settings;
	delete core;
}
