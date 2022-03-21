@echo off
echo.

color

call "%~dp0\..\project\modules\userenv.bat" || goto FAILED

set Path=^
%CD%\..\project\modules;^
%QTDIR%\bin;^
%OPENCV248%\x64\vc9\bin;^
%CODESYN_ODB_PGSQL231%\bin64;^
%CODESYN_ODB_COMMON230%\bin64;^
%PGSQL9331%\lib;%PGSQL9331%\bin;^
%QJSON%\src;^
%QWT610%\lib;^
%GDAL1111%\Debug\bin;^
%Path% || goto FAILED

goto SUCCESS


rem ---------------------------------------------------------------------------
:FAILED

  color C7
rem  echo FAILED
rem  pause

  exit /B 1


rem ---------------------------------------------------------------------------
:SUCCESS

rem  echo SUCCESS
rem  pause

  exit /B 0



