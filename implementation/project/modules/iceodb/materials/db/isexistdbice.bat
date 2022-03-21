@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem Скрипт проверяет есть ли на СУБД PostgreSQL
rem БД ЛЁД
rem
rem
rem Скрипту передаются параметры в следующем порядке:
rem
rem	1. Путь к папке bin установленной директории СУБД PostgreSQL
rem   (Если в пути есть пробелы, путь необходимо заключить в двойные ковычки)
rem	2. Пользователь PostgreSQL
rem	3. Пароль пользователя PostgreSQL
rem 4. Имя базы данных
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

rem Это переменная окружения PostgreSQL
set PGPASSWORD=%3

rem Проверяем есть ли БД ЛЁД в СУБД PostgreSQL
%1\psql -U%2 -d%4 -c"select version()" 2>&0 1>&0

if "%errorlevel%"=="0" goto SUCCESS
if "%errorlevel%" neq "0" goto FAILED


:FAILED
  echo.
  echo ____________
  echo %~nx0
  echo FAILED!!!
  color C7

  exit /B 1


:SUCCESS
  echo.
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07

  exit /B 0
