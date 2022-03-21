@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem Скрипт удаления БД ЛЁД
rem
rem Скрипту передаются параметры в следующем порядке:
rem
rem	1. Пароль пользователя PostgreSQL
rem
rem
rem Возвращаемое значение:
rem
rem	1. 0 - БД ЛЁД удалена успешно (или не было необходимости ее удалять, 
rem	т.к. ее не было)
rem	2. 1 - Ошибка удаления БД ЛЁД
rem
rem ----------------------------------------------------------------------------
rem ВНИМАНИЕ:
rem
rem	Проверка на корректность передаваемых параметров 
rem	не производится, все они должны быть правильные
rem ----------------------------------------------------------------------------

rem Установка переменных окружения пользователя
call "%~dp0\..\..\..\userenv.bat" || goto FAILED
  
call dropdbice "%PGSQL9331%\bin" postgres postgres icedb || goto FAILED

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
