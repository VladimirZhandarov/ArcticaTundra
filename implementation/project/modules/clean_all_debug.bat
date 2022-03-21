@echo off
@echo .

color

call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64 || goto FAILED

rem Настройка среды для сборки
call "%~dp0\adaptive.bat" || goto FAILED

qmake "CONFIG-=debug release debug_and_release debug_and_release_target" "CONFIG+=debug" -Wall -Wparser -Wlogic -Wdeprecated project.pro || goto FAILED


nmake distclean || goto FAILED
del /Q /F ..\*.*


goto SUCCESS


rem ---------------------------------------------------------------------------
:FAILED

  color C7
  echo FAILED
  pause

  exit /B 1


rem ---------------------------------------------------------------------------
:SUCCESS

  echo SUCCESS
  pause

  exit /B 0
