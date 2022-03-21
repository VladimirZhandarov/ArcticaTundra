@echo off
setlocal enabledelayedexpansion 


color

rem ����������� ������ ��
call "%~dp0\version.bat" || goto FAILED

rem ��� ����������� �������� ������
call "%~dp0\adaptive.bat" || goto FAILED

rem ��� ����������� ��������� �������
call "%~dp0\..\..\run\common_run.bat" || goto FAILED

rem ---------------------------------------------------------------------------
rem �������� ����� ������� � ������ ������� VS2008 �� ���� �������
rem �� ������ pro-������
qmake -tp vc "CONFIG-=debug release debug_and_release debug_and_release_target" "CONFIG+=debug_and_release" -r -Wall -Wlogic -Wdeprecated -Wparser project.pro || goto FAILED




rem ---------------------------------------------------------------------------
rem �� ���� ������ ������� VS2008 ���� ������� ���� ������ Win32 �� x64
Echo (*) 
for /d %%A in (*) do (

  for %%B in (%%A\*.vcproj) do (

    rem ������ ���� ������ Win32" � �� ������ Win32, ��� ������� ���, ���
    rem ������ ���������� ��� ����� ��������. ����, ��������, � ���������� �������������
    rem ����� ������ ������ WIN32, �� ��� ������ ������ Win32 ���� ������ ���� ���������.
    rem �.�. ������ ����� � ��������������� ������������ ����� vcproj � Win32 �� x64, ��
    rem ���������� ��� ���� ����� ��������, ��� ������������ ������������� �������� ". ����
    rem � ��������������.
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
