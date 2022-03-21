@echo off
echo.

rem ----------------------------------------------------------------------------
rem
rem ��������� ���������� ��������� VERSION, �������� ������� ����� ���������
rem ������ ������������ ��������
rem
rem ----------------------------------------------------------------------------
rem ��������:
rem
rem ----------------------------------------------------------------------------

rem ����������� ������� ������� �� � Mercurial
set REVISION_CURRENT=NONE
for /f "delims=: tokens=1-3" %%A in ('hg summary') do (if "%%A"=="parent" set REVISION_CURRENT=%%B)
if "%REVISION_CURRENT%" == "NONE" (echo ERROR define CURRENT REVISION !!! & goto FAILED)

rem ����������� ������� ������ � �������, � ������� ��� ������� ������ ������ �� ����� VERSION
set BASE_VERSION=NONE
set REVISION_PREV=NONE
for /f "eol=# delims=| skip=1 tokens=1,2" %%A in (%~dp0\..\..\..\setup\VERSION) do (set BASE_VERSION=%%A& set REVISION_PREV=%%B)
if "%REVISION_PREV%" == "NONE" (echo ERROR define PREVIOUS REVISION !!! & goto FAILED)
if "%BASE_VERSION%" == "NONE" (echo ERROR define CURRENT BASE VERSION !!! & goto FAILED)

rem ������ �������������� �������
set /a REVISION_REAL=%REVISION_CURRENT%-%REVISION_PREV%

rem ��������� ������ ������ 
set VERSION=%BASE_VERSION%-%REVISION_REAL%

exit /B 0