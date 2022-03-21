!include nsDialogs.nsh


# � ������ ���������� ����� ���������� ��������� ���� ��� ��������� �� �����
# ������������� ��� ���������� ����� ��� ������ ���������� (����� �����������)
# ���� ���� ������ ���������� ���)
var DirRequest_State
# ���� ��� ��������� �� ����� �� ���������
!define DirRequest_State_Default "C:\Data"


#------------------------------------------------------------------------------
# ������� ������� ������� ���� PostgreSQL 9.3.3-1 ���� ������ ���� PostgreSQL ��
# �������
#
# �o���������� ��������: ��� ������������� ������� ������� ���� PostgreSQL
#                        9.3.3-1 ����� ����������� ������
#------------------------------------------------------------------------------
!macro RunPostgreSQL913
Function RunPostgreSQL913

  ClearErrors

  DetailPrint "������ ������� ���� PostgreSQL 9.3.3-1"
  FindProcDLL::FindProc "pg_ctl.exe"
  StrCmp $R0 "1" yes no
  yes: # ���� PostgreSQL 9.3.3-1 �������

    DetailPrint "������ ���� PostgreSQL 9.3.3-1 �������"
    goto done

  no: # ���� PostgreSQL 9.3.3-1 �� �������

    DetailPrint "������ ���� PostgreSQL 9.3.3-1 �� �������"
    ExecWait "net start postgresql-x64-9.3"
    ifErrors yes1 no1
    yes1:
      DetailPrint "������ ������� ������� ���� PostgreSQL 9.3.3-1"
      SetErrors # ������������� ������
      goto done
    no1:
      DetailPrint "������ ������� ���� PostgreSQL 9.3.3-1 ������ �������"

  done:

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ��������� � ����������� ������� ������������� ���� PostgreSQL 9.3.3-1
# ���� � ������� ���� �� �����, �� ��� ���������������.
#
# ������������ ��������: � ����� ����� ��������� ����, ��� � �������
#                        ����������� ���� PostgreSQL 9.3.3-1
#                        ��� ������ ��������� ���� PostgreSQL 9.3.3-1 �����
#                        ����������� ������
#------------------------------------------------------------------------------
!macro InstallPostgreSQL913
Function InstallPostgreSQL913

  DetailPrint "��������� ���� PostgreSQL 9.3.3-1"

  # ����������, ����������� �� ���� PostgreSQL 9.3.3-1
  Var /GLOBAL path
  ClearErrors
  ReadRegStr $path HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes no
  yes: # ���� PostgreSQL 9.3.3-1 �� �����������
    DetailPrint "���� PostgreSQL 9.3.3-1 �� �����������"
    # ������������� ���� PostgreSQL 9.3.3-1
    SetOutPath "$TEMP\${_COMPANY_NAME_FS_}"
    File "opo\postgresql-9.3.3-1-windows-x64.exe"
    
    # ���� ����� ���� � ��������� �� �� ��������� (���������� ����� ���������� ����
    # ��������� ���� � ��, ������� ����� ���� ��. ����), �� ��������� ���� �� ���������
    StrCmp $DirRequest_State "" y n
    y:
      StrCpy $DirRequest_State ${DirRequest_State_Default}
    n:
    
    ExecWait "$TEMP\${_COMPANY_NAME_FS_}\postgresql-9.3.3-1-windows-x64.exe --unattendedmodeui minimal \
             --mode unattended --servicepassword postgres --superpassword postgres \
             --serverport 5432 --install_runtimes 1 --datadir $DirRequest_State"
    # ��� �������� ��������� �������� --datadir $DirRequest_State
    # ������� � ���, ��� �� ��������� ����������� PostgreSQL ������������� ���� �
    # ����� data, ������� ����������� � ��������� ������ Windows. �� ���������
    # ��� ���� ����� ������� ������������� ����� ����� �� ������, �.�. �����������
    # �� ����� � ����� data ��������� �������������� ����� (pg_hba.conf � �.�.)
    # - ��� ��������� - �� ��������� ������ PostgreSQL.
    RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
    
    # ��� ��� ������ ������ �������������� ���� PostgreSQL 9.3.3-1
    # ��� ����, ����� ���������, ��� �� ����������.
    ClearErrors
    ReadRegStr $path HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
    ifErrors yes1 done
    yes1:
      DetailPrint "������ ��������� ���� PostgreSQL 9.3.3-1"
      SetErrors
      goto done

  no: # ���� PostgreSQL 9.3.3-1 �����������
    DetailPrint "���� PostgreSQL 9.3.3-1 �����������"

  done:

  # ���������� ����, �� �������� ����������� ���� PostgreSQL 9.3.3-1
  Push $path

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ��������� ������ ����������� ������� ���� PostgreSQL
#
# �������� ��������: � ����� ������ ������ ����, �� �������� ���� PostgreSQL
#                    ����������� � �������
#
# �o���������� ��������: ��� ������������� ����������� ������ �������
#                        ���� PostgreSQL ����� ����������� ������
#                        ������ ����� �������� � ����
#------------------------------------------------------------------------------
!macro GetVersionPostgreSQL
Function GetVersionPostgreSQL

  ClearErrors
  
  Var /GLOBAL pth
  Pop $pth

  System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PGPASSWORD", "postgres").r0'
  SetOutPath "$TEMP\${_COMPANY_NAME_FS_}"
  ExecWait 'cmd /C $\"$\"$pth$\"\bin\psql -Upostgres --command "select version()"$\" > version.txt' $R0
  ClearErrors
  FileOpen $0 version.txt r
  IfErrors yes no
  yes:

    DetailPrint "������ �������� ����� $OutDir\version.txt"
    goto error

  no:

    FileRead $0 $1 # ����� ������
    FileRead $0 $1 # ��������� ��
    FileRead $0 $1 # ������ �������
    ${WordFind} $1 "," "+01" $R0
    DetailPrint "������ ���� PostgreSQL: $R0"
    FileClose $0
    RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
    ClearErrors
    goto done

  error:
    SetErrors

  done:
    Push $R0

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ����, ���������������� ������������ ����� ����� ��������� �� �����
#------------------------------------------------------------------------------
!macro SetDir4DataPage
Function SetDir4DataPage

  # �������� �������� ������� �����, ����� � ��������������� ���������� �� ������
  # ��������� PostgreSQL
  SectionGetFlags ${secPostgreSQL} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} equal less more
  less:
  more:
    Abort
  equal:
  
  # �������� �������� ������� �����, ����� PostgreSQL �����������
  ClearErrors
  ReadRegStr $0 HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes1 no1
  no1: # PostgreSQL �����������
    goto InstallNotRequired # ���������� ����, ��� ��������� � ���, ���
                            # postgresql-x64-9.1 �����������
  yes1: # PostgreSQL �� �����������
                          
  !insertmacro MUI_HEADER_TEXT "��������� ����������, ��� ����� �������� ������ \
                                ��" ""

  var /GLOBAL Dialog
  var /GLOBAL Label
  var /GLOBAL DirRequest
  var /GLOBAL BrowseButton

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateLabel} 0 0 100% 14u "�������� ����������, ��� ����� �������� \
                                   ������ �� �� $\"${_MODULE_FULL_NAME_}$\"."
  Pop $Label
	
  ${NSD_CreateLabel} 0 14u 100% 28u "��������: �� ����������, ����� ���������� \
                                     ���������� � ����� C:\Program Files\ ��� \
                                     C:\Program Files(x86)\."
  Pop $Label
  
  ${NSD_CreateGroupBox} 0 42u 100% 35u "����� ���������"
  Pop $Label
	
  StrCmp $DirRequest_State "" yes no
  yes:
    StrCpy $DirRequest_State ${DirRequest_State_Default} # ������� ��� ���������
                                                         # �� ���������
  no:
  ${NSD_CreateDirRequest} 7u 56u -70u 14u $DirRequest_State
  Pop $DirRequest
  ${NSD_SetFocus} $DirRequest
	
  ${NSD_CreateBrowseButton} -60u 56u 53u 14u "����� ..." # 53u ��������� � �����
                                                         # �.�. ����� -60u
  Pop $BrowseButton
  ${NSD_OnClick} $BrowseButton OnClick_BrowseButton
  
  goto showDialog
  
  # ���������� ����, ��� ��������� � ���, ���
  # postgresql-x64-9.1 �����������
  InstallNotRequired:
  
    !insertmacro MUI_HEADER_TEXT "���� PostgreSQL 9.3.3-1 \
                                  ��� ����������." ""

    nsDialogs::Create 1018
    Pop $Dialog

    ${If} $Dialog == error
      Abort
    ${EndIf}

    ${NSD_CreateLabel} 0 0 100% -13u "���� PostgreSQL 9.3.3-1 ��� ���������� � �������. \
                                      ����� ������������ �������� �� ������������� ������ \
                                      � ���������� ������ ���� PostgreSQL 9.3.3-1."
    Pop $Label
  
  goto showDialog
  
  showDialog:

  nsDialogs::Show

FunctionEnd
#------------------------------------------------------------------------------
Function OnClick_BrowseButton

  nsDialogs::SelectFolderDialog /NOUNLOAD \
    "�������� ���������� ��� ��������� ..." $DirRequest_State
  Pop $0
  ${If} $0 == "error" # ������������ ������� ����� �����
    Return
  ${EndIf}
  ${If} $0 != ""
    system::Call `user32::SetWindowText(i $DirRequest, t "$0")`
  ${EndIf}
  
FunctionEnd
#------------------------------------------------------------------------------
Function leaveSetDir4DataPage

  # ���������� ��������� ������� � ����� ��� �������������� � ������,
  # ���� ������������ ������ ������ "�����"
  ${NSD_GetText} $DirRequest $DirRequest_State

FunctionEnd
!macroend
