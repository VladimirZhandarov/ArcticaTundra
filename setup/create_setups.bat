@echo off
echo.

rem ----------------------------------------------------------------------------
rem
rem Скрипт сборки дистрибутивов проекта
rem
rem ----------------------------------------------------------------------------

rem Определение версии ПО
call "%~dp0\..\implementation\project\modules\version.bat" || goto FAILED

rem Устанавливаем переменные окружения пользователя для данного программного 
rem обеспечения
call "%~dp0\..\implementation\project\modules\userenv.bat" || goto FAILED

rem Данный скрипт может быть вызван с параметром ONLY_VERSION для того чтобы
rem это скрипт сформировал только VERSION, но не собирал дистрибутив. VERSION
rem нужна, например, при сборке конечного приложения для отображения версии
rem в заголовке главного окна приложения.
if "%1"=="ONLY_VERSION" exit /B 0
NSIS\makensis /V4 projectapp_d.nsi   || goto FAILED

goto SUCCESS

:FAILED
  echo.
  echo ____________
  echo %~nx0
  echo FAILED!!!
  color C7

  pause

  exit /B 1


:SUCCESS
  echo.
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07

  pause

  exit /B 0

