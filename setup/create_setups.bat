@echo off
echo.

rem ----------------------------------------------------------------------------
rem
rem ������ ������ ������������� �������
rem
rem ----------------------------------------------------------------------------

rem ����������� ������ ��
call "%~dp0\..\implementation\project\modules\version.bat" || goto FAILED

rem ������������� ���������� ��������� ������������ ��� ������� ������������ 
rem �����������
call "%~dp0\..\implementation\project\modules\userenv.bat" || goto FAILED

rem ������ ������ ����� ���� ������ � ���������� ONLY_VERSION ��� ���� �����
rem ��� ������ ����������� ������ VERSION, �� �� ������� �����������. VERSION
rem �����, ��������, ��� ������ ��������� ���������� ��� ����������� ������
rem � ��������� �������� ���� ����������.
if "%1"=="ONLY_VERSION" exit /B 0
NSIS\makensis /V4 projectapp_d.nsi   || goto FAILED

goto SUCCESS

:FAILED
  echo.
  echo ____________
  echo %~nx0
  echo FAILED!!!
  color C7

  pause

  exit /B 1


:SUCCESS
  echo.
  echo ____________
  echo %~nx0
  echo SUCCESS!!!
  color 07

  pause

  exit /B 0

