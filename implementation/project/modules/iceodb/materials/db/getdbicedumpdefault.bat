@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem Скрипт получения дампа БД ЛЁД
rem
rem Скрипту передаются параметры в следующем порядке:
rem
rem	1. Пароль пользователя PostgreSQL
rem
rem
rem Возвращаемое значение:
rem 
rem	В папке запуска скрипта будет создан файл dbicedump.sql, содержащий
rem	дамп БД ЛЁД.
rem
rem ----------------------------------------------------------------------------
rem ВНИМАНИЕ:
rem
rem	Проверка на корректность передаваемых параметров 
rem	не производится, все они должны быть правильные
rem ----------------------------------------------------------------------------

rem Установка переменных окружения пользователя
call "%~dp0\..\..\..\userenv.bat" || goto FAILED
  
call getdbicedump "%PGSQL9331%\bin" postgres postgres icedb || goto FAILED

goto SUCCESS

:FAILED

  echo.
  echo ____________
  echo %~nx0
  echo FAILED!!!
  color C7
  echo.

  pause

  exit /B 1


:SUCCESS

  echo.
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07
  echo.

  pause

  exit /B 0
