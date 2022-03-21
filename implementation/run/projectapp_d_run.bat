@echo off
echo.

color

call "%~dp0\common_run.bat" || goto FAILED
    
start "" "%~dp0\..\project\tundraapp_d.exe" || goto FAILED

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
rem  pause

  exit /B 0



