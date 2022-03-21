@echo off
echo.

rem ----------------------------------------------------------------------------
rem
rem Переменные среды пользователя
rem
rem ----------------------------------------------------------------------------
rem ВНИМАНИЕ:
rem
rem В данном файле пользователь должен изменить пути к переменным окружения
rem в зависимости от того, где у него в системе установлены 
rem стороннии пограммные библиотеки
rem ----------------------------------------------------------------------------

set CODESYN_ODB_BIN230=C:\DVG\Distr_Arctica\odb-2.3.0-i686-windows
set CODESYN_ODB_COMMON230=C:\DVG\Distr_Arctica\libodb-2.3.0
set CODESYN_ODB_PGSQL231=C:\DVG\Distr_Arctica\libodb-pgsql-2.3.1
set OPENCV248=C:\DVG\Distr_Arctica\opencv\build
set CODESYN_ODB_QT231=C:\DVG\Distr_Arctica\libodb-qt-2.3.1
set PGSQL9331=C:\Program Files\PostgreSQL\9.3
set QTDIR=C:\DVG\Distr_Arctica\qt-everywhere-opensource-src-4.8.6
set BOOST1470=C:\DVG\boost_1_47_0
set QJSON=C:\DVG\Distr_Arctica\qjson-master
set QWT610=C:\DVG\Distr_Arctica\qwt-6.1.0
set CUDA_TOOL55=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v5.5
set OPENCV248=C:\DVG\Distr_Arctica\opencv\build
set GDAL1111=C:\DVG\Distr_Arctica\gdal1111\gdal-1.11.1
rem set QMAKESPEC=%QTDIR%\mkspecs\win32-msvc2008

exit /B 0