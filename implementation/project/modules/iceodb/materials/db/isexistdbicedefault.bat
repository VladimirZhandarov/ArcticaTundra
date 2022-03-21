@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem Скрипт проверяет есть ли на ЛЁД PostgreSQL
rem БД ЛЁД
rem
rem
rem Скрипту передаются параметры в следующем порядке:
rem
rem	1. Путь к папке bin установленной директории ЛЁД PostgreSQL
rem    (Если в пути есть пробелы, путь необходимо заключить в двойные ковычки)
rem	2. Пароль пользователя PostgreSQL
rem
rem
rem Возвращаемое значение:
rem 
rem	1. 0 - БД ЛЁД существует
rem	2. 1 - БД ЛЁД НЕ существует
rem
rem ----------------------------------------------------------------------------
rem ВНИМАНИЕ:
rem
rem	Проверка на корректность передаваемых параметров 
rem	не производится, все они должны быть правильные
rem ----------------------------------------------------------------------------

rem Установка переменных окружения пользователя
call "%~dp0\..\..\..\userenv.bat" || goto FAILED
  
call isexistdbice "%PGSQL9331%\bin" postgres postgres icedb || goto FAILED

goto SUCCESS

:FAILED

  echo.
  echo Ѓ„ ‹…„ ­Ґ бгйҐбвўгҐв
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07
  echo.

  pause

  exit /B 1


:SUCCESS

  echo.
  echo Ѓ„ ‹…„ бгйҐбвўгҐв
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07
  echo.

  pause

  exit /B 0
