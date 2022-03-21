@echo off
echo.


rem ----------------------------------------------------------------------------
rem
rem ������ ��������� ���� �� �� ���� PostgreSQL
rem �� ˨�
rem
rem
rem ������� ���������� ��������� � ��������� �������:
rem
rem	1. ���� � ����� bin ������������� ���������� ���� PostgreSQL
rem   (���� � ���� ���� �������, ���� ���������� ��������� � ������� �������)
rem	2. ������������ PostgreSQL
rem	3. ������ ������������ PostgreSQL
rem 4. ��� ���� ������
rem
rem
rem ������������ ��������:
rem 
rem	1. 0 - �� ˨� ����������
rem	2. 1 - �� ˨� �� ����������
rem
rem ----------------------------------------------------------------------------
rem ��������:
rem
rem	�������� �� ������������ ������������ ���������� 
rem	�� ������������, ��� ��� ������ ���� ����������
rem ----------------------------------------------------------------------------

rem ��� ���������� ��������� PostgreSQL
set PGPASSWORD=%3

rem ��������� ���� �� �� ˨� � ���� PostgreSQL
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
