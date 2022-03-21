@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem Скрипт создает БД ЛЁД в СУБД PostgreSQL.
rem Если БД ЛЁД уже существует в PostgreSQL,
rem скрипт перед созданием удалит БД ЛЁД.
rem
rem Скрипту передаются параметры в следующем порядке:
rem
rem	1. Слово setup, которое используется при запуске скрипта из дистрибутива
rem    программы с целью отключения необходимости пользователю нажимать
rem    любую кнопку для закрытия консоли
rem
rem Возвращаемое значение:
rem 
rem	1. 0 - БД ЛЁД создана и инициализирована успешно
rem	2. 1 - Ошибка создания и ининциализации БД ЛЁД
rem
rem ----------------------------------------------------------------------------
rem ВНИМАНИЕ:
rem
rem	Проверка на корректность передаваемых параметров 
rem	не производится, все они должны быть правильные
rem ----------------------------------------------------------------------------

rem Перенаправления ошибок в файл error.txt
if "%STDOUT_REDIRECTED%" == "" (
    set STDOUT_REDIRECTED=yes
    cmd.exe /c %0 %1 2> error.txt
    exit /b %ERRORLEVEL%
)

if NOT "%1"=="setup" (
  rem Установка переменных окружения пользователя
  call "%~dp0\..\..\..\userenv.bat" || goto FAILED 
)

rem Создаем структуру БД ЛЁД, накатываем классификаторы

call initdbice "%PGSQL9331%\bin" postgres postgres icedb || goto FAILED

goto SUCCESS

:FAILED

  echo.
  echo ____________
  echo %~nx0
  echo FAILED!!!
  color C7
  echo.

if NOT "%1" == "setup" (

  pause

  exit /B 1

) else (

  exit 1

)


:SUCCESS

  echo.
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07 
  echo.

if NOT "%1" == "setup" (

  pause

  exit /B 0

) else (

  exit 0

)
