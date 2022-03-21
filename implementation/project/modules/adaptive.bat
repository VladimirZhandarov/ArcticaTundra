@echo off
echo.

rem ----------------------------------------------------------------------------
rem
rem Скрипт формирования среды только для СБОРКИ ПО
rem 
rem
rem ----------------------------------------------------------------------------
rem ВНИМАНИЕ:
rem
rem ----------------------------------------------------------------------------

rem Установка переменных окружения пользователя
call "%~dp0\userenv.bat" || goto FAILED

rem %QTDIR%\bin в переменной среды окружения Path прописывается только лишь 
rem для того, чтобы в результате сборки был доступен файл qmake.exe. С этой же
rem целью прописывается и %CODESYN_ODB_BIN230%\bin - для возможности запуска 
rem компилятора odb в процессе сборки
set Path=^
%QTDIR%\bin;^
%CODESYN_ODB_BIN230%\bin;^
%CUDA_TOOL55%\bin;^
%Path%

rem Заголовочные файлы сторонних библиотек зависят от других заголовочных файлов
rem сторонних библиотек - надо эти пути обеспечить
set INCLUDE=^
%PGSQL9331%\include;^
%CODESYN_ODB_COMMON230%;^
%INCLUDE%
set LIB=^
%QTDIR%\lib;^
%PGSQL9331%\lib;^
%CODESYN_ODB_COMMON230%\lib64;^
%LIB%

set GDAL_FILENAME_IS_UTF8=NO

goto SUCCESS

rem ---------------------------------------------------------------------------
:FAILED

rem  color C7
rem  echo FAILED
rem  pause

  exit /B 1


rem ---------------------------------------------------------------------------
:SUCCESS

rem  echo SUCCESS
rem  pause

  exit /B 0
