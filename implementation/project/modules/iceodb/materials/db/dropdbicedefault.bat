@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem ������ �������� �� ˨�
rem
rem ������� ���������� ��������� � ��������� �������:
rem
rem	1. ������ ������������ PostgreSQL
rem
rem
rem ������������ ��������:
rem
rem	1. 0 - �� ˨� ������� ������� (��� �� ���� ������������� �� �������, 
rem	�.�. �� �� ����)
rem	2. 1 - ������ �������� �� ˨�
rem
rem ----------------------------------------------------------------------------
rem ��������:
rem
rem	�������� �� ������������ ������������ ���������� 
rem	�� ������������, ��� ��� ������ ���� ����������
rem ----------------------------------------------------------------------------

rem ��������� ���������� ��������� ������������
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
