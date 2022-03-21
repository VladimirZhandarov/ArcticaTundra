#==============================================================================
# ��������:        ����� ������, ������� � ������� ��� �������� ������
#                  �������������
# ��������� �����: ---
#==============================================================================
#
# �����:     $Author: shchav $
# ������:    2011.11.08
# �������:   $Date: 2012-03-06 15:06:30 +0300 (Вт, 06 мар 2012) $
# ���������: Intel Windows 7 Professional SP1 x64
#
#------------------------------------------------------------------------------
# ����������:
#------------------------------------------------------------------------------
# ��������:
#-----------------------------------------------------------------------------


#------------------------------------------------------------------------------
# ����������� ��������������� ���������
#------------------------------------------------------------------------------
!include "WordFunc.nsh"
!include "FileFunc.nsh"

!include "x64.nsh"


#------------------------------------------------------------------------------
# ������������� ���������
#------------------------------------------------------------------------------
# ������ ������������ ������
!define _MODULE_FULL_NAME_ "${_MODULE_NAME_}-${_SOFT_VERSION_}"
# ������������ ����� ������������
!define _INSTALLER_NAME_ "${_MODULE_NAME_FS_}-${_SOFT_VERSION_}.exe"
# ��������� ����� ���� � �������� ������� �� �������� ������ ������ ����� ���������
# � ����������� ������ ������
!define _PATH_FS_ "${_COMPANY_NAME_FS_}\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}"
# ��������� ����� ���� � ���� "�����������" �� ������� ������ ����� ���������
!define _PATH_ "${_COMPANY_NAME_}\${_SYSTEM_NAME_}\�� $\"${_MODULE_FULL_NAME_}$\""


#------------------------------------------------------------------------------
# ��������� ������� ���������
#------------------------------------------------------------------------------
# ����� ��������� ������ ������ ������������
SetCompressor /SOLID lzma
# �������� ������� �� �������� ����������� �����������
Name "�� $\"${_MODULE_FULL_NAME_}$\""
# ��������� ���� ������������
Caption "��������� �� $\"${_MODULE_FULL_NAME_}$\""
# ������������ ����� ������������
OutFile "${_INSTALLER_NAME_}"
# ������� ��������� �� ���������
InstallDir "$PROGRAMFILES64\${_PATH_FS_}"
# ���������� �������� �������� ������������/��������������
ShowInstDetails show
ShowUninstDetails show


#------------------------------------------------------------------------------
# ������� � �������
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# ������������� �����������
#------------------------------------------------------------------------------
Function .onInit

  ClearErrors

  ${GetParameters} $R1
  ${GetOptions} "$R1" "/?" $R0
  IfErrors nohelp help
help:
  MessageBox MB_OK "Options$\n \
                   /? - help$\n \
                   /chos=[yes|no] - manage checking OS$\n \
                   $\n"
  Abort
nohelp:

  ClearErrors

  # ��������� ����� ���� �������� ������ ���������������
  Push '������ ������������ �� ������ "��������������" \
        ����� ����� �� ��������� ������������ �����������'
  Call checkUser

  # ����������� �� ���������� ��������� ������ ������������� �������� ��
  ${GetOptions} "$R1" "/chos=" $R0
  StrCmp $R0 "no" nochos chos
chos:
  # ��������� ������ �������� �� ������������ �� Windows
  GetVersion::WindowsName
  Pop $R0
  StrCmp $R0 "${_WINDOWS_NAME_}" 0 error
  #GetVersion::WindowsType
  #Pop $R1
  #StrCmp $R1 "${_WINDOWS_TYPE_}" 0 error
  #GetVersion::WindowsServicePack
  #Pop $R2
  #StrCmp $R2 "${_WINDOWS_SERVICE_PACK_}" 0 error
  GetVersion::WindowsPlatformArchitecture
  Pop $R3
  StrCmp $R3 "${_WINDOWS_PLATFORM_ARCHITECTURE_}" 0 error
nochos:
  # ��� �������� ������ ������������ �������, ���� ����� ��������� ���������
  # �� �������� ����� ������������������� ����� ���� � ������� ���� ���������
  # "error" �� "0 #error"
  #MessageBox MB_ICONSTOP|MB_OK "��: $R0, ���: $R1, SP: $R2, ���������: $R3"
  #Goto error

  # ����� �� ������� ������ ��������� ��������������� � ����� C:\Programm Files,
  # � �� � C:\Programm Files(x86)
  ${DisableX64FSRedirection}

  Goto noerrors

  error:

    MessageBox MB_ICONSTOP|MB_OK "${_MODULE_NAME_} ����� ���� ����������� ������ �� \
      Windows ${_WINDOWS_NAME_} ${_WINDOWS_TYPE_} ${_WINDOWS_SERVICE_PACK_} \
      (${_WINDOWS_PLATFORM_ARCHITECTURE_} bit)."
    abort

  noerrors:

  # ��� ������� � ������� �� 64-������ ���������
  SetRegView 64
  
  ClearErrors

FunctionEnd


#------------------------------------------------------------------------------
# ������������� �������������
#------------------------------------------------------------------------------
Function un.onInit

  # �������� ����� ���� �������� ������ ���������������
  Push '������ ������������ �� ������ "��������������" \
        ����� ����� �� �������� ������������ �����������'
  Call un.checkUser
  
  ClearErrors

  # ��� ������� ������������� � ������ silent ��� ����� ���� ����������
  # �������� DUS, �������� �������� ���������� �������� �������� ������������
  # ��. ��� /DUS=0 ��� ��� ���������� - ��������� ������������ ����� �������,
  # ��� ������ ��������� ��������� ������������ �� ����� �������.
  # DUS(DeleteUserSettings)
  ${GetParameters} $R1
  ${GetOptions} "$R1" "/DUS" $R0
  Var /GLOBAL DUS
  IfErrors noDUS DUS
DUS:
  StrCpy $DUS $R0
  goto +3
noDUS:
  StrCpy $DUS 0
  
  # ��� ������� � ������� �� 64-������ ���������
  SetRegView 64
 
  ClearErrors

FunctionEnd


#------------------------------------------------------------------------------
# ������� �������� ������� ������� ������ �� ������������ "��������������"
#
# �������� ��������: � ����� ������ ������ ���������, ������� ������������
#                    � ������, ���� ������� ������� ������ �� ��������
#                    "���������������"
#------------------------------------------------------------------------------
!macro checkUser UN
Function ${UN}checkUser

  ClearErrors

  Pop $0

  UserInfo::GetName
  Pop $1
  UserInfo::GetAccountType
  Pop $2
  UserInfo::GetOriginalAccountType
  Pop $3
  StrCmp $2 "Admin" +3 0
    MessageBox MB_ICONSTOP|MB_OK $0
  Abort "��������� ����� ��������"
  
  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� �������� ����������� ������ ������, ������� ��������� � �����
# /AppData/Roaming/DVG/savoi/savoidmdapp ������� ������������.
# ��������� ��� ��� ������� ������ ��� ������� ������������, �� ��� ��������
# ��� ������� ���� ������.
#------------------------------------------------------------------------------
!macro UN.DeleteSettings
Function un.DeleteSettings

  ClearErrors

  # ��������� ������� (D:\Users) �� ������� ������������
  # �������������, �.�. ���������� ����������� ������ �� ��� ����.
  ${WordFind} $PROFILE "\" "-1{" $R5

  # ��������, ������� �� ������������� � ������ silent
  ifSilent yes no
  yes:
    StrCmp $DUS 0 false true
  no:
    MessageBox MB_YESNO "������� ��������� ���������� ��� ���� ������������� \
                         �������?" IDYES true IDNO false
  true:
    # ������� � ���� �������� ��� �������� AppData (�� ����� �������,
    # ������� ������������� ��������� ����������� �����������) � ������������
    # ��� � ������� un.delSets
    ClearErrors
    ${Locate} "$R5" "/L=D /M=${_COMPANY_NAME_FS_}" "un.delSets"
  false:
  
  ClearErrors

FunctionEnd
#------------------------------------------------------------------------------
Function un.delSets

  ClearErrors

  RMDir /r "$R9\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" # ������� ��������� ������
  ifErrors 0 +2
  MessageBox MB_OK "������ �������� ���������������� ������ �������� �� $\"${_MODULE_FULL_NAME_}$\": \
					$\n $\"$R9\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}$\" \
					$\n��� �������� ���������������� ������ �������� ������������ \ 
					�������������� ������ ������� ������ �������."
  # � ����������� ������� ����� ���� � ������ ������. ���� �� ���, ��
  # ���������� ��������� ��� ��������, ������� ���� ������� ��� ��������
  # �������� ������.
  RMDir "$R9\${_SYSTEM_NAME_FS_}"
  ifErrors +2 0
  RMDir "$R9"

  Push $0 # ��. help �� Locate ��� ������� ��������� ������

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ���������/�������� ����: "����" ��� ������
#------------------------------------------------------------------------------
!macro SetupStartMenu UN
Function ${UN}SetupStartMenu

  ClearErrors

!if "${UN}" != "un."

  # ��������� ������ � ��������� �� ���� "����"
  CreateDirectory "$SMPROGRAMS\${_PATH_}"
  CreateShortCut "$SMPROGRAMS\${_PATH_}\�� $\"${_MODULE_FULL_NAME_}$\".lnk" \
                 "$INSTDIR\${_RUNNING_FILE_}" "" "$INSTDIR\${_RUNNING_FILE_}" 0 \
                  SW_SHOWNORMAL ALT|CONTROL|SHIFT|A "�� $\"${_MODULE_FULL_NAME_}$\""
  CreateShortCut "$SMPROGRAMS\${_PATH_}\������� �� $\"${_MODULE_FULL_NAME_}$\".lnk" \
                 "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 \
                 SW_SHOWNORMAL ALT|CONTROL|SHIFT|U "������� �� $\"${_MODULE_FULL_NAME_}$\""

!else

  # �������� ������ � ��������� �� ���� "����"
  RMDir /r "$SMPROGRAMS\${_PATH_}"
  # � ����������� ������� ����� ���� � ������ ������. ���� �� ���, ��
  # ���������� ��������� ��� ������, ������� ���� ������� ��� ��������
  # ��������� ������ ������.
  RMDir "$SMPROGRAMS\${_COMPANY_NAME_}\${_SYSTEM_NAME_}"
  ifErrors +2 0
  RMDir "$SMPROGRAMS\${_COMPANY_NAME_}"

!endif

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ���������/�������� ������ ��������� �� ������� �����
#------------------------------------------------------------------------------
!macro SetupDesktop UN
Function ${UN}SetupDesktop

  ClearErrors

!if "${UN}" != "un."

  CreateShortCut "$DESKTOP\�� ${_MODULE_FULL_NAME_}.lnk" \
                 "$INSTDIR\${_RUNNING_FILE_}"
  
!else

  Delete "$DESKTOP\�� ${_MODULE_FULL_NAME_}.lnk"

!endif
  
  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� �����������/������������� ����� ��������� ������ � �������
#------------------------------------------------------------------------------
!macro RegisterModule UN
Function ${UN}RegisterModule

  ClearErrors

!if "${UN}" != "un."

  WriteRegStr HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
    "Base Directory" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
    "Branding" "${_MODULE_NAME_FS_}"
  WriteRegStr HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
    "Version" "${_SOFT_VERSION_}"

!else

  # ������� �� ������� ����������� ����� ��������� ������
  DeleteRegKey HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}"
  DeleteRegKey /ifempty HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}"
  DeleteRegKey /ifempty HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations"
  DeleteRegKey /ifempty HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\"

!endif

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� �����������/������������� ������ � ������ "��������� � ��������
# ��������"
#------------------------------------------------------------------------------
!macro RegisterModuleToAddRemoveProgramm UN
Function ${UN}RegisterModuleToAddRemoveProgramm

  ClearErrors

!if "${UN}" != "un."

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "DisplayName" "�� $\"${_MODULE_NAME_}$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "DisplayIcon" "$\"$INSTDIR\${_RUNNING_FILE_}$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "Publisher" "OOO $\"${_COMPANY_NAME_}$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "HelpLink" "${_COMPANY_E_MAIL_}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "DisplayVersion" "${_SOFT_VERSION_}"

!else

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}"

!endif

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������ �������� ������ �� ������������ ����� � �������� �������
#
# ����� ������� ��� ������ �����, ������� ����������� ���� ���������
# ������������ �����
#------------------------------------------------------------------------------
!macro DeleteModuleFromFS

  ClearErrors
  
  # �������� ������ � ��������� �� �������� �������, ������� ���� ������� ���
  # ���������
  RMDir /r $INSTDIR
  ifErrors done 0
  # ������� ���������� ��� ������ �����, ������� ����������� ����
  # ������������ �����
  StrCpy $0 "\.."
  loop:
    DetailPrint $INSTDIR$0
    RMDir $INSTDIR$0
    ifErrors done 0
    ${WordInsert} "$0" "\" "..\" "-1" $0
    goto loop
  done:
  
  ClearErrors

!macroend


#------------------------------------------------------------------------------
# ������� ��������� ������������� ������ ������ � ��������������� ������ ������
#
# �o���������� ��������: � ����� ����� ��������� ��������:
#                        1. -1 - ������ � ������� �� ����������;
#                            0 - ������ ���������;
#                            1 - ����������� ������ ������;
#                            2 - ����������� ����� ����� ������;
#                        2. ������ �������������� ������
#                          (���� ������ �� ����������, �� "")
#------------------------------------------------------------------------------
Function IsModuleInstalled

  ClearErrors
  ReadRegStr $0 HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
    "Version"
  IfErrors yes no
  yes: # ������ � ������� �� ����������
    Push "" # ����� � ���� ������ �������������� ������
    Push -1
    goto exit
  no: # ��������� ������ ���������������� ������ � ���������������
    Push $0 # ����� � ���� ������ �������������� ������
    ${WordFind} $0 "-" "+01" $R0 # 1.52.43-109 ����� 1.52.43 � ������������� ������
    ${WordFind} ${_SOFT_VERSION_} "-" "+01" $R1 # 1.52.43-109 ����� 1.52.43
    ${VersionCompare} "$R0" "$R1" $R2
    IntCmpU $R2 0 equal less more
    equal:
      ${WordFind} $0 "-" "+02" $R0 # 1.52.43-109 ����� 109 � ������������� ������
      ${WordFind} ${_SOFT_VERSION_} "-" "+02" $R1 # 1.52.43-109 ����� 109
      IntCmpU $R0 $R1 equal1 less more1
      equal1:
        Push 0
        DetailPrint "������ ���������: $0 ${_SOFT_VERSION_}"
        goto exit
      less:
        Push 1
        DetailPrint "����������� ������ ������: $0 ${_SOFT_VERSION_}"
        goto exit
      more1:
        Push 2
        DetailPrint "����������� ����� ����� ������: $0 ${_SOFT_VERSION_}"
        goto exit
    more:
      IntCmpU $R2 1 more1 less less

    exit:

FunctionEnd


#------------------------------------------------------------------------------
# ������� ��������� �������� ��������� ��������������� ������ ������ � �������
# ������ ������ ������
#
# �������� ��������: 1. � ������� ����� ������ ������ ���������, �������
#                       ������������ � ������, ���� ������������ �������
#                       ������ ������
#                    2. ���� � ����� ������ ������ ���������, �������
#                       ������������ � ������, ���� ������������ ����� ������
#                       ������
#                    3. ��� ���� � ����� ������ ������ ���������, �������
#                       ������������ � ������, ���� ������������ ����� ��
#                       ������ ������
# ��� ������������� ������ ������ � ������������� ������ ������������� ������
# ����� �������.
#------------------------------------------------------------------------------
!macro HandleCompareVersionModule
Function HandleCompareVersionModule

  ClearErrors
  Call IsModuleInstalled
  Pop $0
  Pop $1 # �������� �� ����� ������������� ������ ������

  Pop $R3 # ���������� ��������� ��� ������������� ������� ������ ������
  Pop $R2 # ���������� ��������� ��� ������������� ������ ������ ������
  Pop $R1 # ���������� ��������� ��� ���������� ������ �������

  IntCmp $0 0 0 less more # ������ ���������
    MessageBox MB_YESNO $R1 IDYES reinstall IDNO 0
    Quit
  more:
    IntCmp $0 1 0 less1 more1 # ����������� ������ ������
    MessageBox MB_YESNO $R2 IDYES reinstall IDNO 0
    Quit
    more1:
    less1: # ����������� ������� ������
      MessageBox MB_YESNO $R3 IDYES reinstall IDNO 0
    Quit

    reinstall:
      # ������� ������������� ������
      ReadRegStr $0 HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
        "Base Directory"
      MessageBox MB_YESNO "������� ��������� ���������� ��� ���� ������������� \
                           �������?" IDYES true IDNO false
      true:
        ExecWait '"$0\uninstall.exe" /S /DUS=1 /SMS'
        # ����� ������� ��������������� ���������� ���������,
        # ���� ������������� �������� ������.
        FP1:
          FindProcDLL::FindProc "Au_.exe"
          StrCmp $R0 "1" FP1 less
      false:
        ExecWait '"$0\uninstall.exe" /S /SMS'
        # ����� ������� ��������������� ���������� ���������,
        # ���� ������������� �������� ������.
        FP2:
          FindProcDLL::FindProc "Au_.exe"
          StrCmp $R0 "1" FP2 less

    less:
    
    ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ������� ����������, ���� ����� ������� �� ��������� �������� �
# ������������ ������� ����������
#------------------------------------------------------------------------------
!macro RunSoft
Function RunSoft

  SetOutPath "$INSTDIR"
  ExecShell open "${_RUNNING_FILE_}"

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ������� ������� ���������� ���������� � ������� ������ ������ ���������� �
# ���� Detailview NSIS. ������� ��������� ���������� ���������.
# (��� ������� ����������� ��� ������� bat-������, �.�. ��� �� �������
# � ������� ExecWait �� ���������� ������������ �������� �� bat-������,
# ������ 1)
#
# �������� ��������: � ����� ������ ��������� ����������� ��������� ����������
#
# �o���������� ��������: � $0 ����� ��������� ��������� ����� ����������
#                        ���������� � ����� ������
#------------------------------------------------------------------------------
!macro RunWait
Function RunWait

  GetFunctionAddress $0 OutputToDetailView #(OutputToDetailView - ��. ����)
  Pop $1
  ExecDos::exec /TOFUNC $1 '' $0
  
FunctionEnd
#------------------------------------------------------------------------------
Function OutputToDetailView
  Pop $0
  DetailPrint $0
FunctionEnd
!macroend


#------------------------------------------------------------------------------
# ����������� ��������
#------------------------------------------------------------------------------
# ������ ������������/�������������� ������ ������������ �� ���
# ������� ������� "��������������"
!insertmacro checkuser ""
!insertmacro checkuser "un."
