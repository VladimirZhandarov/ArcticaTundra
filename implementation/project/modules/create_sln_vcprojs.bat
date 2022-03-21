@echo off
setlocal enabledelayedexpansion 


color

rem Определение версии ПО
call "%~dp0\version.bat" || goto FAILED

rem Для обеспечения успешной сборки
call "%~dp0\adaptive.bat" || goto FAILED

rem Для обеспечения успешного запуска
call "%~dp0\..\..\run\common_run.bat" || goto FAILED

rem ---------------------------------------------------------------------------
rem Создание файла решения и файлов проекта VS2008 во всех модулях
rem на основе pro-файлов
qmake -tp vc "CONFIG-=debug release debug_and_release debug_and_release_target" "CONFIG+=debug_and_release" -r -Wall -Wlogic -Wdeprecated -Wparser project.pro || goto FAILED




rem ---------------------------------------------------------------------------
rem Во всех файлов проекта VS2008 всех модулей идет замена Win32 на x64
Echo (*) 
for /d %%A in (*) do (

  for %%B in (%%A\*.vcproj) do (

    rem Замена идет именно Win32" а не просто Win32, это вызвано тем, что
    rem замена происходит без учета регистра. Если, например, в директивах препроцессора
    rem будет указан макрос WIN32, то при замене просто Win32 этот макрос тоже заменится.
    rem Т.к. задача стоит в переопределении конфигурации файла vcproj с Win32 на x64, то
    rem исследовав это файл можно заметить, что конфигурация заканчивается символом ". Этим
    rem и вопользовались.
    for /F "tokens=* delims=," %%C in (%%B) do (
      set LINE=%%C
      set TMPR=!LINE:Win32"=x64"!
      Echo !TMPR! >> TMP.TXT
    )
    move TMP.TXT %%B

  )

)


goto SUCCESS


rem ---------------------------------------------------------------------------
:FAILED

  color C7
  pause

  exit /B 1


rem ---------------------------------------------------------------------------
:SUCCESS

  start project.sln

  exit /B 0
