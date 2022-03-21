#==============================================================================
# ��������:        ������ ������ ������������ ��� ���
# ��������� �����: ��� "���" DeveloperGroup (�. �����-���������)
#==============================================================================
#
# �����:     $Author: shchav $
# ������:    2011.11.08
# �������:   $Date: 2012-03-06 15:06:30 +0300 (Вт, 06 мар 2012) $
# ������:    Colos
# ���������: Intel Windows 7 Professional SP1
#
#------------------------------------------------------------------------------
# ����������:
#------------------------------------------------------------------------------
# ��������:
#-----------------------------------------------------------------------------

#------------------------------------------------------------------------------
# ����������� ��������������� ���������
#------------------------------------------------------------------------------
# ����������� "������������" ����������
!include "MUI2.nsh"


#------------------------------------------------------------------------------
# ��������� ������
#------------------------------------------------------------------------------
# ������������ ������
!define _MODULE_NAME_ "������"
!define _MODULE_NAME_FS_ "tundraapp"
!define _RUNNING_FILE_ "tundraapp_d.exe" # ����, ������� �����������
                                      # �� ��������� ���������


#------------------------------------------------------------------------------
# ������������ �������
#------------------------------------------------------------------------------
!include "projectfncsprt.nsh"
!include "postgresql.nsh"


#------------------------------------------------------------------------------
# ��������� ������������ ����������
#------------------------------------------------------------------------------
# ��������� ��������� ������� ������ ����������� �� ������������� ����� �������

# ������ ������������
!define MUI_ICON "..\implementation\project\modules\iceapp\iceapp\images\installicon.ico"

# ������ ��������������
!define MUI_UNICON "..\implementation\project\modules\iceapp\iceapp\images\uninstallicon.ico"

# ���� �������� � ����������� �����������
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\implementation\project\modules\iceapp\iceapp\images\installpage.bmp"

# ���� �������� � ����������� �������������
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\implementation\project\modules\iceapp\iceapp\images\uninstallpage.bmp"

#------------------------------------------------------------------------------
# ��������, ������� ����� ����� ��������� ������������ ��� �����������
#, ������� �� ������������ ����� ��������

!define MUI_ABORTWARNING # �������� �������������� ��� �������� ������������
                         # �������������, ��������, "�� ������������� ������"
                         # �������� ���������" � �.�.

# �������� �����������
!define MUI_WELCOMEPAGE_TITLE_3LINES # ����� ����������� �������
!insertmacro MUI_PAGE_WELCOME # �������� ����������

# �������� ��������� ���������
!insertmacro MUI_PAGE_COMPONENTS # �������� ��������� ���������

# �������� ��������� ���� � �����, ��� ����� ������� ��
# (��� ������� ��� ����� data)
PageEx custom
  PageCallbacks SetDir4DataPage leaveSetDir4DataPage
PageExEnd

# �������� ��������� ����������
!insertmacro MUI_PAGE_DIRECTORY # �������� ��������� ����������

# �������� ��������� ���������� ������������
!insertmacro MUI_PAGE_INSTFILES # �������� ��������� ���������� ������������

# �������� ���������� ������ �����������
!define MUI_FINISHPAGE_TITLE_3LINES # ����� ������� �������
!define MUI_FINISHPAGE_RUN_TEXT "��������� �� $\"${_MODULE_FULL_NAME_}$\""
!define MUI_FINISHPAGE_RUN $INSTDIR\${_RUNNING_FILE_} # ����� ��������� ������� �� ���������
                                                      # �������� � ������������ �������
                                                      # ����������
!define MUI_PAGE_CUSTOMFUNCTION_SHOW mui_page_finish_show # ��� ����, ����� ����� ������� �������
                                                          # ���������. ��� ������ �������������
                                                          # ������������.
!insertmacro MUI_PAGE_FINISH # �������� ���������� ������ �����������

#------------------------------------------------------------------------------
# ��������, ������� ����� ����� ��������� ������������ ��� �������������
#, ������� �� ������������ ����� ��������

!define MUI_UNABORTWARNING # �������� �������������� ��� �������� ������������
                           # �������������, ��������, "�� ������������� ������"
                           # �������� ���������" � �.�.

# �������� ����������� �������������
!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_UNPAGE_WELCOME # �������� ����������� �������������

# �������� �������� ���������
!insertmacro MUI_UNPAGE_COMPONENTS # �������� �������� ���������

# �������� ������������� ��������
!insertmacro MUI_UNPAGE_CONFIRM # �������� ������������� ��������

# �������� �������� �������� �������������
!insertmacro MUI_UNPAGE_INSTFILES # �������� �������� �������� ������������

# �������� ���������� ������ �������������
!define MUI_FINISHPAGE_TITLE_3LINES # ����� ������� �������
!insertmacro MUI_UNPAGE_FINISH # �������� ���������� ������ �������������

!insertmacro MUI_LANGUAGE "Russian" # ��� �������� ������ ���� �� ������� �����


#------------------------------------------------------------------------------
# ������ ��������� ���������� ��� ������
#------------------------------------------------------------------------------
Section "�� ${_MODULE_NAME_}" secTundraApp

  # ��������� ������������ ��� ���� �������������
  SetShellVarContext all

  # ���������� ������ �������������� ������ � �������
  # (���� ��, ������� ����������)
  Call IsModuleInstalled
  Pop $0
  Pop $1 # �������� �� ����� ������������� ������ ������

  # ������������ ���� �������/���������� ������������� ������ ��
  Push "�� $\"${_MODULE_NAME_}$\" ��� �����������. \
        $\n������ �������������� �� ($1) ��������� � ��������������� (${_SOFT_VERSION_}). \
        $\n�������������� �� $\"${_MODULE_NAME_}$\" (${_SOFT_VERSION_})?"
  Push "����������� ����� ������ ������ �� $\"${_MODULE_NAME_}$\" ($1). \
        $\n�������������� �� $\"${_MODULE_NAME_}$\" (${_SOFT_VERSION_})?"
  Push "����������� ����� ����� ������ �� $\"${_MODULE_NAME_}$\" ($1). \
        $\n�������������� �� $\"${_MODULE_NAME_}$\" (${_SOFT_VERSION_})?"
  Call HandleCompareVersionModule

  # ����������� �� ������� ����� Visual Studio
  SetOutPath "$TEMP\${_COMPANY_NAME_FS_}"
  # ��� ������ ������ ���������� � ������ debug
  File "opo\Visual C++ 2008 Redistributable Debug (x64)\mergemoduledebugsetup(x64).msi"
  ExecWait "msiexec.exe /i $\"$TEMP\${_COMPANY_NAME_FS_}\mergemoduledebugsetup(x64).msi$\" /qn /passive \
            ALLUSERS=1"
  RMDir /r "$TEMP\${_COMPANY_NAME_FS_}\"

  SetOutPath "$INSTDIR"
  # ����������� �� ������� �������
  !define PROJECTDIR "..\implementation\project"
  file "${PROJECTDIR}\*.dll"
  file "${PROJECTDIR}\${_RUNNING_FILE_}"

  # ����������� �� ������� CUDA
  !define CUDA_TOOL55 "$%CUDA_TOOL55%\bin"
  file "${CUDA_TOOL55}\cudart64_55.dll"
  file "${CUDA_TOOL55}\nppi64_55.dll"
  file "${CUDA_TOOL55}\nppc64_55.dll"
  file "${CUDA_TOOL55}\npps64_55.dll"
  file "${CUDA_TOOL55}\cufft64_55.dll"

  # ����������� �� ������� opencv
  !define OPENCV248 "$%OPENCV248%\x64\vc9\bin"
  file "${OPENCV248}\opencv_core248d.dll"
  file "${OPENCV248}\opencv_highgui248d.dll"
  file "${OPENCV248}\opencv_imgproc248d.dll"
  file "${OPENCV248}\opencv_gpu248d.dll"
  file "${OPENCV248}\opencv_flann248d.dll"
  file "${OPENCV248}\opencv_features2d248d.dll"
  file "${OPENCV248}\opencv_calib3d248d.dll"
  file "${OPENCV248}\opencv_video248d.dll"
  file "${OPENCV248}\opencv_objdetect248d.dll"

  # ����������� �� ���������� GDAL
  !define GDAL1111 "$%GDAL1111%"
  file "${GDAL1111}\Debug\bin\gdal111.dll"
  file "${GDAL1111}\Debug\bin\gdal_translate.exe"

  # ����������� �� ������ CODESYN_ODB_PGSQL231
  !define CODESYN_ODB_PGSQL231 "$%CODESYN_ODB_PGSQL231%\bin64"
  file "${CODESYN_ODB_PGSQL231}\odb-pgsql-d-2.3-vc9.dll"

  # ����������� �� ������ CODESYN_ODB_COMMON230
  !define CODESYN_ODB_COMMON230 "$%CODESYN_ODB_COMMON230%\bin64"
  file "${CODESYN_ODB_COMMON230}\odb-d-2.3-vc9.dll"

  # ����������� �� ��������� PostgreSQL 9.3.3-1
  !define PGSQL9331 "$%PGSQL9331%\bin"
  file "${PGSQL9331}\libpq.dll"
  file "${PGSQL9331}\ssleay32.dll"
  file "${PGSQL9331}\libeay32.dll"
  file "${PGSQL9331}\libintl-8.dll"
  #file "${PGSQL9331}\libiconv-2.dll"

  # ����������� �� ���������� QJSON
  !define QJSON "$%QJSON%"
  file "${QJSON}\src\qjsond.dll"

  #SetOutPath "$INSTDIR\testdata"
  ## �������� ������
  #!define TESTDATA "..\..\testdata\"
  #File "${TESTDATA}\IMG_0651.JPG"

  # ������� ���������
  #File /r /x ".svn" "${SOIIDIR}\modules\soiikpmsrvapp\materials\doc"

  SetOutPath "$INSTDIR"
  # ����������� �� Qt ���������
  !define QTDIR473 "$%QTDIR%"
  File "${QTDIR473}\bin\QtCored4.dll"
  File "${QTDIR473}\bin\QtGuid4.dll"
  File "${QTDIR473}\bin\QTSVGD4.DLL"
  File "${QTDIR473}\bin\QTOPENGLD4.DLL"
  File "${QTDIR473}\bin\QtNetworkd4.dll"
  File "${QTDIR473}\bin\QtWebKitd4.dll"
  File "${QTDIR473}\bin\QtXmld4.dll"

  SetOutPath "$INSTDIR"
  # ����������� �� Qwt ����������
  !define QWT610 "$%QWT610%"
  File "${QWT610}\lib\qwtd.dll"

  # ���������������� ����� �� ���������
  #!define CONFIGFILE "..\savoi\modules\savoiclntapp\materials\config"
  #file /r "${CONFIGFILE}\*.*"

  # ������������� �������������
  # ��� ���������� ���������� ������ ��� ����� "SetupStartMenu".
  # ����� ������ ��� ������ ������������� �� ����� �����, ����� ������� ���������� �
  # ����� uninstall.exe ��� �������� 0.
  WriteUninstaller "uninstall.exe"

  # ��������� ���� "����"
  Call SetupStartMenu

  # ��������� ������ �� ������� �����
  Call SetupDesktop

  # ������������ ���� ��������� ������ � �������
  Call RegisterModule

  # ������������ � ������ "��������� � ��������
  # ��������"
  Call RegisterModuleToAddRemoveProgramm

  # �� ��������� ���� ������������� (��� ������� ������ �������� �����������)
  SetAutoClose false

SectionEnd

#------------------------------------------------------------------------------
# ������ ��������� PostgreSQL 9.3.3-1
#------------------------------------------------------------------------------
Section "PostgreSQL 9.3.3-1" secPostgreSQL

  # ��������� ������������ ��� ���� �������������
  SetShellVarContext all

  # ������������� ���� PostgreSQL 9.3.3-1, ���� ��� �� ����� � �������
  Call InstallPostgreSQL913
  Var /GLOBAL baseDirPostgreSQL
  Pop $baseDirPostgreSQL
  ifErrors yes2 no2
  yes2:
    MessageBox MB_OK "������ ��������� ���� PostgreSQL 9.3.3-1 \
                      $\n��������� ����� ��������."
    Quit
  no2:

  # ��������� ���� PostgreSQL ���� �� �� �������
  Call RunPostgreSQL913
  ifErrors yes no
    yes:
      MessageBox MB_OK "������ ������� ���� PostgreSQL 9.3.3-1 \
                        $\n��������� ����� ��������."
      Quit
    no:

  # ����������� ������ ����������� ���� PostgreSQL
  Push $baseDirPostgreSQL
  Call GetVersionPostgreSQL
  Var /GLOBAL Version
  Pop $Version
  ifErrors yes1 no1
  yes1:
    DetailPrint "������ ����������� ������ ������� ���� PostgreSQL"
    MessageBox MB_OK "������ ����������� ������ ������� ���� PostgreSQL. \
                      $\n ��������� ����� ��������."
    Quit
  no1:
    StrCmp $Version " PostgreSQL 9.3.3" 0 +3
    DetailPrint "������ ���� PostgreSQL ����������"
    Goto done1
    DetailPrint "������ ���� PostgreSQL �� ����������"
    MessageBox MB_OK "������ ����������� ������� ���� PostgreSQL: $\"$Version$\".\
                      $\n�� �������������: $\"PostgresSQL 9.3.3-1$\". \
                      $\n��������� ����� ��������."
    Quit
  done1:

  # �� ��������� ���� ������������� (��� ������� ������ �������� �������������)
  SetAutoClose false

SectionEnd


#------------------------------------------------------------------------------
# ������ ��������� �� ˨� � �� ������
#------------------------------------------------------------------------------
Section "���� ������" secDataBase

  # ��������� ������������ ��� ���� �������������
  SetShellVarContext all

  # ����������, ����������� �� ���� PostgreSQL 9.3.3-1
  Var /GLOBAL pathPostgreSQL
  ClearErrors
  ReadRegStr $pathPostgreSQL HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes no
  yes: # ���� PostgreSQL 9.3.3-1 �� �����������
    MessageBox MB_OK "������ �������� �� �� $\"${_MODULE_FULL_NAME_}$\" � ���� PostgreSQL 9.3.3-1. \
                      $\n���� PostgreSQL 9.3.3-1 �� �����������. \
                      $\n���������� ���� PostgreSQL 9.3.3-1. \
                      $\n��������� ����� ��������."
    Quit
  no: # ���� PostgreSQL 9.3.3-1 �����������

  # ��������� ���� PostgreSQL ���� �� �� �������
  Call RunPostgreSQL913
  ifErrors yes6 no6
  yes6:
    MessageBox MB_OK "������ ������� ���� PostgreSQL 9.3.3-1. \
                      $\n��������� ����� ��������."
    Quit
  no6:

  # �������� �� icedb
  # �����������, ���������� �� �� ��� � ����
  System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PGPASSWORD", "postgres").r0'
  ExecDos::exec /DETAILED 'cmd /C $\"$\"$pathPostgreSQL$\"\bin\psql -Upostgres -dicedb --command "select version()"$\"'
  Pop $R0
  ClearErrors
  IntCmp $R0 0 yes2 no2 no2
  no2: # �� ��� � ���� �����������
    # ������� �� ��� � ����
    DetailPrint "�� icedb � ���� PostgresSQL 9.3.3-1 ���"
    ExecDos::exec /DETAILED 'cmd /C $\"$\"$pathPostgreSQL$\"\bin\psql -Upostgres --command "create database icedb"$\"'
    Pop $R0
    ClearErrors
    IntCmp $R0 0 yes3 no3 no3
    yes3: # �������� �� ��� � ���� ������ �������
      DetailPrint "�� icedb � ���� PostgresSQL 9.1.3 ������� �������"

      # ������� ��������� �� ���, ���������� ��������������, ���������� �������� ������
      System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PGSQL9331", "$pathPostgreSQL").r0' # ��� ����� ���
                                                                                                 # �������� ������
      !define MODULESICEODB "..\implementation\project\modules\iceodb"
      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb"
      # ������� �������� ��������� �� ���
      file "${MODULESICEODB}\iceodb\*.sql"

      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb\iceodb"
      # �������� ������ �������� � ������ ������ �� ���
      file "${MODULESICEODB}\materials\db\initdbicedefault.bat"
      file "${MODULESICEODB}\materials\db\initdbice.bat"
      file "${MODULESICEODB}\materials\db\dropdbice.bat"
      file "${MODULESICEODB}\materials\db\isexistdbice.bat"
      file "${MODULESICEODB}\materials\db\setdbicetestdata.bat"
      file "${MODULESICEODB}\materials\db\sqls.txt"

      # ������� ������ �������� ������
      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb\iceodb\data"
      file "${MODULESICEODB}\materials\db\data\*.sql"

      # ��������� �������� ������ �������� � ������ ������ �� �����
      ClearErrors
      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb\iceodb"
      Push "$OUTDIR\initdbicedefault.bat setup"
      Call RunWait
      DetailPrint "Pop $0"
      StrCmp $0 "SUCCESS!!!" yes33 no33
      yes33: # �������� ��������� � ������ ������ �� ��� � ���� ������ �������
        DetailPrint "�� icedb � ���� PostgresSQL 9.3.3-1 ������� �����������"
        RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
        goto done2
      no33: # ������ �������� ��������� � ������ ������ �� ��� � ����
        MessageBox MB_YESNO  "������ �������� ��������� � ������ ������ �� �� $\"${_MODULE_FULL_NAME_}$\" \
                              $\n� ���� PostgreSQL 9.3.3-1. \
                              $\n������� ����� �� ������? \
                              $\n(����� ���������� ��������� ������������� ��� ������� \
                              $\n�� ������: ${_COMPANY_E_MAIL_})." IDYES yes44 IDNO no44
        yes44:
          ExecShell open error.txt
          Sleep 1000 # ��� ����, ����� ������ ����������� ���������� �������,
                     # ��� �� ���� ���������� ����������.
        no44:
          RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
        goto no3
    no3:
      MessageBox MB_OK "������ �������� �� �� $\"${_MODULE_FULL_NAME_}$\" � ���� PostgreSQL 9.3.3-1. \
                        $\n��������� ����� ��������."
      Quit
  yes2: # �� ����� � ���� ������������
    DetailPrint "�� icedb � ���� PostgresSQL 9.3.3-1 ������������"
    MessageBox MB_YESNO "�� �� $\"${_MODULE_FULL_NAME_}$\" � ���� PostgreSQL 9.3.3-1 ������������. \
                         $\n�������������� �� �� $\"${_MODULE_FULL_NAME_}$\"?" \
                         IDYES yes4 IDNO done2
    yes4: # �������� �� ����� �� ����
      DetailPrint "�������� �� icedb � ���� PostgresSQL 9.3.3-1"
      ExecDos::exec /DETAILED 'cmd /C $\"$\"$pathPostgreSQL$\"\bin\psql -Upostgres --command "drop database icedb"$\"'
      Pop $R0
      ClearErrors
      IntCmp $R0 0 yes5 no5 no5
      yes5: # �������� �� ��� �� ���� ������ �������
        DetailPrint "�� icedb � ���� PostgresSQL 9.3.3-1 ������� �������"
        goto no2 # ������� �� �������� �� ��� � ����
      no5: # ������ �������� �� ��� �� ����
        MessageBox MB_OK "������ �������� �� �� $\"${_MODULE_FULL_NAME_}$\" � ���� PostgreSQL 9.3.3-1. \
                          $\n��������� ����� ��������."
        Quit

  done2:

  # �� ��������� ���� ������������� (��� ������� ������ �������� �������������)
  SetAutoClose false

SectionEnd


#------------------------------------------------------------------------------
# ������ �������� ���������� ��� ������
#------------------------------------------------------------------------------
Section "un.${_MODULE_NAME_}" secUNTundraApp

  # �������� ������������ ��� ���� �������������
  SetShellVarContext all

  # �������� ������ �� ������� �����
  Call un.SetupDesktop

  # ������� ������ �� ���� "����"
  Call un.SetupStartMenu

  # ������� ����������� � ������ "��������� � ��������
  # ��������"
  Call un.RegisterModuleToAddRemoveProgramm

  # ������� ������ �� ������������ ����� � �������� �������
  !insertmacro DeleteModuleFromFS

  # ������� ����������� ������ � �������
  Call un.RegisterModule

  # �� ��������� ���� ������������� (��� ������� ������ �������� �������������)
  SetAutoClose false

SectionEnd


#------------------------------------------------------------------------------
# ������ �������� PostgreSQL 9.3.3-1
#------------------------------------------------------------------------------
Section /o "un.PostgreSQL 9.3.3-1" secUNPostgreSQL

  # PostgreSQL 9.3.3-1 �� ������ ��������� � ������ Silent
  ifSilent 0 +2
    return

  # ����������, ����������� �� ���� PostgreSQL 9.3.3-1 � ���
  ClearErrors

  ReadRegStr $pathPostgreSQL HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes no
  no: # ���� PostgreSQL 9.3.3-1 ����������� � ����������� �� ���� $pathPostgreSQL

    # ���������� ���� � ��������, ��� ��������� �� (���� ���� �� ��� ������)
    # ��� ���������� ���������� ������, ��� ��� ������ � ��� � ������� ��������� ����� �������� ����
    ReadRegStr $0 HKLM SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3\ "Data Directory"
    ifErrors 0 +2
        MessageBox MB_OK "������ ����������� �������� �������� �� �� $\"${_MODULE_FULL_NAME_}$\" \
                          $\n������� ������ �� �����."

    # ���������� ���� � ��������, ��� ��������� ���� (���� ���� �� ��� ������)
    # ��� ���������� ���������� ������, ��� ��� ������ � ��� � ������� ��������� ����� �������� ����
    # ���� ������� ���������� ����� ������� �������������, �.�. ������������� ���� ����� ������ �� 
    # ������
    ReadRegStr $1 HKLM SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3\ "Base Directory"
    ifErrors 0 +2
        MessageBox MB_OK "������ ����������� �������� ��������� ���� �� $\"${_MODULE_FULL_NAME_}$\" \
                          $\n������� ��������� ���� ������ �� �����."

    # ������� ����
    ExecWait "$pathPostgreSQL\uninstall-postgresql.exe --unattendedmodeui minimal \
              --mode unattended"
    Sleep 5000 # ��� ����, ����� ������ ����������� ���������� �������,
               # ��� �� ���� ���������� ����������.
    # ������� ���� �� �������� ������� ���������, ���� ��� ��������� ������� ��������� ����
    StrCmp $1 "" +4 0
    RMDir /r $1"\.." # ��������� ������� ���������� �� ������� ���� ����, ��� ������� �� �������
    ifErrors iy in
    iy: DetailPrint "������ �������� �������� ��������� ����: $\"$1$\""
        goto id
    in: DetailPrint "������� ��������� ����: $\"$1$\" ������ �������"
    id:

    # ������� ��
    MessageBox MB_YESNO  "������� ���� ������ �� $\"${_MODULE_FULL_NAME_}$\"? \
                          $\n$\n��������: �������� ���� ������ ��������� ��� ������ ��������!!!" \
                          IDYES yes55 IDNO yes
    # ������� ��, ���� ��������� ������ ������� (��. ����)
    yes55: StrCmp $0 "" +4 0
           RMDir /r $0
           ifErrors 0 +2
           MessageBox MB_OK "������ �������� �������� �������� �� �� $\"${_MODULE_FULL_NAME_}$\": \
                             $\n $\"$0$\" \
                             $\n��� ������� �������� ���� PostgreSQL 9.3.3-1 ������������ \ 
                             �������������� ������ ������� ������ �������."

  yes: # ���� PostgreSQL 9.3.3-1 �� �����������

  ClearErrors
  
SectionEnd


#------------------------------------------------------------------------------
# ���������������� ������� ��� ����������� �������� ������ ����� ���������
# ����������� �������� ������� �������
#------------------------------------------------------------------------------
Function mui_page_directory_pre

  # �������� �������� ������� �����, ����� � ��������������� ���������� �� ������
  # ��������� �� ������, ���� �� �� � ���� ���������.
  SectionGetFlags ${secTundraApp} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} equal less more
  less:
  more:
    Abort
  equal:

FunctionEnd


#------------------------------------------------------------------------------
# ���������������� ������� ��� ����������� ��������� �������� 
#------------------------------------------------------------------------------
Function mui_page_finish_show

  # �� ��������� ������� ������� ��������� ������ ���� ����, �� ������ ���������������
  # �������������.
  SendMessage $mui.FinishPage.Run ${BM_SETCHECK} ${BST_UNCHECKED} 0

  # ��� ����, ����� ������ ������� ������� ���������, ���� �� ��������������� ���
  # �� ������ (�� ������ �� ������� �����������), ��������� �� ������.
  SectionGetFlags ${secTundraApp} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} equal less more
  less:
  more:
    ShowWindow $mui.FinishPage.Run ${SW_HIDE}
  equal:

FunctionEnd


#------------------------------------------------------------------------------
# �������� �����������
#------------------------------------------------------------------------------
LangString DESC_secTundraApp ${LANG_RUSSIAN} "�� $\"${_MODULE_FULL_NAME_}$\""
LangString DESC_secPostgreSQL ${LANG_RUSSIAN} "���� PostgreSQL 9.3.3-1"
LangString DESC_secDataBase ${LANG_RUSSIAN} "�� �� $\"${_MODULE_FULL_NAME_}$\". \
                                             ��������������� �� \
                                             ��������� ���� PostgreSQL 9.3.3-1 \
                                             ��������� ������������ postgres c \
                                             ������� postgres"
# ��� ����������� ������������
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${secTundraApp} $(DESC_secTundraApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${secPostgreSQL} $(DESC_secPostgreSQL)
  !insertmacro MUI_DESCRIPTION_TEXT ${secDataBase} $(DESC_secDataBase)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

# ��� ����������� ��������������
!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${secTundraApp} $(DESC_secTundraApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${secPostgreSQL} $(DESC_secPostgreSQL)
#  !insertmacro MUI_DESCRIPTION_TEXT ${secDataBase} $(DESC_secDataBase)
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END


#------------------------------------------------------------------------------
# ����������� ��������
#------------------------------------------------------------------------------
# �������� ����������� ������ ������
!insertmacro UN.DeleteSettings

# �����������/������������� ����� ��������� ������ � �������
!insertmacro RegisterModule ""
!insertmacro RegisterModule "un."

# ���������/�������� ����: "����" ��� ������
!insertmacro SetupStartMenu ""
!insertmacro SetupStartMenu "un."

# ���������/�������� ������ �� ������� �����
!insertmacro SetupDesktop ""
!insertmacro SetupDesktop "un."

# ������� �����������/������������� ������ � ������ "��������� � ��������
# ��������"
!insertmacro RegisterModuleToAddRemoveProgramm ""
!insertmacro RegisterModuleToAddRemoveProgramm "un."

# ��������� ��������������� ������ ������ � ������������� ������ ������
!insertmacro HandleCompareVersionModule

# ��������� � ����������� ������� ������������� ���� PostgreSQL 9.3.3-1
!insertmacro InstallPostgreSQL913

# ������ ������� ���� PostgreSQL 9.3.3-1 ���� ������ ���� PostgreSQL ��
# �������
!insertmacro RunPostgreSQL913

# ��������� ������ ����������� ������� ���� PostgreSQL
!insertmacro GetVersionPostgreSQL

# ��� ������� ���������� ���������� � ���������� ������ � ���������
# ���������� ������ ���������� ���������� � �������
!insertmacro RunWait

# ������� ����������� �������� (��. ���� � ���� �����)
# ��������� ����, ���� ����� ��������������� �� ���
!insertmacro SetDir4DataPage
