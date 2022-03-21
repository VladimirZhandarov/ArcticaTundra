@echo off
echo.

rem ----------------------------------------------------------------------------
rem
rem ������ ������������ ����� ������ ��� ������ ��
rem 
rem
rem ----------------------------------------------------------------------------
rem ��������:
rem
rem ----------------------------------------------------------------------------

rem ��������� ���������� ��������� ������������
call "%~dp0\userenv.bat" || goto FAILED

rem %QTDIR%\bin � ���������� ����� ��������� Path ������������� ������ ���� 
rem ��� ����, ����� � ���������� ������ ��� �������� ���� qmake.exe. � ���� ��
rem ����� ������������� � %CODESYN_ODB_BIN230%\bin - ��� ����������� ������� 
rem ����������� odb � �������� ������
set Path=^
%QTDIR%\bin;^
%CODESYN_ODB_BIN230%\bin;^
%CUDA_TOOL55%\bin;^
%Path%

rem ������������ ����� ��������� ��������� ������� �� ������ ������������ ������
rem ��������� ��������� - ���� ��� ���� ����������
set INCLUDE=^
%PGSQL9331%\include;^
%CODESYN_ODB_COMMON230%;^
%INCLUDE%
set LIB=^
%QTDIR%\lib;^
%PGSQL9331%\lib;^
%CODESYN_ODB_COMMON230%\lib64;^
%LIB%

set GDAL_FILENAME_IS_UTF8=NO

goto SUCCESS

rem ---------------------------------------------------------------------------
:FAILED

rem  color C7
rem  echo FAILED
rem  pause

  exit /B 1


rem ---------------------------------------------------------------------------
:SUCCESS

rem  echo SUCCESS
rem  pause

  exit /B 0
