@echo off
@echo .

color

call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64 || goto FAILED

rem Определение версии ПО
call "%~dp0\version.bat" || goto FAILED

rem Настройка среды для сборки
call "%~dp0\adaptive.bat" || goto FAILED

rem Определение версии приложения (будет установлена соответствующая 
rem переменная окружения)
set CurrentDir=%CD% || goto FAILED
cd ..\..\..\setup\ || goto FAILED
call create_setups.bat ONLY_VERSION || goto FAILED
cd %CurrentDir% || goto FAILED

qmake "CONFIG-=debug release debug_and_release debug_and_release_target" "CONFIG+=debug" -Wall -Wparser -Wlogic -Wdeprecated project.pro || goto FAILED

nmake || goto FAILED


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
