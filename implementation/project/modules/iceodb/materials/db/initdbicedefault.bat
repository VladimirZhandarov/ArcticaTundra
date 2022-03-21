@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem ������ ������� �� ˨� � ���� PostgreSQL.
rem ���� �� ˨� ��� ���������� � PostgreSQL,
rem ������ ����� ��������� ������ �� ˨�.
rem
rem ������� ���������� ��������� � ��������� �������:
rem
rem	1. ����� setup, ������� ������������ ��� ������� ������� �� ������������
rem    ��������� � ����� ���������� ������������� ������������ ��������
rem    ����� ������ ��� �������� �������
rem
rem ������������ ��������:
rem 
rem	1. 0 - �� ˨� ������� � ���������������� �������
rem	2. 1 - ������ �������� � �������������� �� ˨�
rem
rem ----------------------------------------------------------------------------
rem ��������:
rem
rem	�������� �� ������������ ������������ ���������� 
rem	�� ������������, ��� ��� ������ ���� ����������
rem ----------------------------------------------------------------------------

rem ��������������� ������ � ���� error.txt
if "%STDOUT_REDIRECTED%" == "" (
    set STDOUT_REDIRECTED=yes
    cmd.exe /c %0 %1 2> error.txt
    exit /b %ERRORLEVEL%
)

if NOT "%1"=="setup" (
  rem ��������� ���������� ��������� ������������
  call "%~dp0\..\..\..\userenv.bat" || goto FAILED 
)

rem ������� ��������� �� ˨�, ���������� ��������������

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
