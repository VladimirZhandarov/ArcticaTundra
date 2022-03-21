!include nsDialogs.nsh


# В данной переменной будет находиться выбранный путь для установки БД САВОИ
# установленный или диалоговым окном или другим источником (кроме диалогового)
# окна пока других источников нет)
var DirRequest_State
# Путь для установки БД САВОИ по умолчанию
!define DirRequest_State_Default "C:\Data"


#------------------------------------------------------------------------------
# Функция запуска сервера СУБД PostgreSQL 9.3.3-1 если сервер СУБД PostgreSQL не
# запущен
#
# Вoзвращаемое значение: При невозможности запуска сервера СУБД PostgreSQL
#                        9.3.3-1 будет установлена ошибка
#------------------------------------------------------------------------------
!macro RunPostgreSQL913
Function RunPostgreSQL913

  ClearErrors

  DetailPrint "Запуск сервера СУБД PostgreSQL 9.3.3-1"
  FindProcDLL::FindProc "pg_ctl.exe"
  StrCmp $R0 "1" yes no
  yes: # СУБД PostgreSQL 9.3.3-1 запущен

    DetailPrint "Сервер СУБД PostgreSQL 9.3.3-1 запущен"
    goto done

  no: # СУБД PostgreSQL 9.3.3-1 НЕ запущен

    DetailPrint "Сервер СУБД PostgreSQL 9.3.3-1 НЕ запущен"
    ExecWait "net start postgresql-x64-9.3"
    ifErrors yes1 no1
    yes1:
      DetailPrint "Ошибка запуска сервера СУБД PostgreSQL 9.3.3-1"
      SetErrors # Устанавливаем ошибку
      goto done
    no1:
      DetailPrint "Запуск сервера СУБД PostgreSQL 9.3.3-1 прошел успешно"

  done:

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция установки и определения наличие установленной СУБД PostgreSQL 9.3.3-1
# если в системе СУБД не стоит, то она устанавливается.
#
# Возвращаемое значение: в стеке будет находится путь, где в системе
#                        установлена СУБД PostgreSQL 9.3.3-1
#                        При ошибке установки СУБД PostgreSQL 9.3.3-1 будет
#                        установлена ошибка
#------------------------------------------------------------------------------
!macro InstallPostgreSQL913
Function InstallPostgreSQL913

  DetailPrint "Установка СУБД PostgreSQL 9.3.3-1"

  # Определяем, установлена ли СУБД PostgreSQL 9.3.3-1
  Var /GLOBAL path
  ClearErrors
  ReadRegStr $path HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes no
  yes: # СУБД PostgreSQL 9.3.3-1 НЕ установлена
    DetailPrint "СУБД PostgreSQL 9.3.3-1 НЕ установлена"
    # Устанавливаем СУБД PostgreSQL 9.3.3-1
    SetOutPath "$TEMP\${_COMPANY_NAME_FS_}"
    File "opo\postgresql-9.3.3-1-windows-x64.exe"
    
    # Если никто путь к установке БД не установил (установить может диалоговое окно
    # установки пути к БД, функцию этого окна см. ниже), то принимаем путь по умолчанию
    StrCmp $DirRequest_State "" y n
    y:
      StrCpy $DirRequest_State ${DirRequest_State_Default}
    n:
    
    ExecWait "$TEMP\${_COMPANY_NAME_FS_}\postgresql-9.3.3-1-windows-x64.exe --unattendedmodeui minimal \
             --mode unattended --servicepassword postgres --superpassword postgres \
             --serverport 5432 --install_runtimes 1 --datadir $DirRequest_State"
    # Был вынужден прописать параметр --datadir $DirRequest_State
    # Связано с тем, что по умолчанию инсталлятор PostgreSQL устанавливает базу в
    # папку data, которая размещается в системных папках Windows. По умолчанию
    # для этих папок система устанавливает права толко на чтение, т.о. инсталлятор
    # не может в папку data прописать дополнительные файлы (pg_hba.conf и т.д.)
    # - как результат - не создается служба PostgreSQL.
    RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
    
    # Еще раз читаем версию установленного СУБД PostgreSQL 9.3.3-1
    # для того, чтобы убедиться, что он установлен.
    ClearErrors
    ReadRegStr $path HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
    ifErrors yes1 done
    yes1:
      DetailPrint "Ошибка установки СУБД PostgreSQL 9.3.3-1"
      SetErrors
      goto done

  no: # СУБД PostgreSQL 9.3.3-1 установлена
    DetailPrint "СУБД PostgreSQL 9.3.3-1 установлена"

  done:

  # Возвращаем путь, по которому установлена СУБД PostgreSQL 9.3.3-1
  Push $path

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция получения версии запущенного сервера СУБД PostgreSQL
#
# Входящее значение: В стеке должен лежать путь, по которому СУБД PostgreSQL
#                    установлена в системе
#
# Вoзвращаемое значение: При невозможности определения версии сервера
#                        СУБД PostgreSQL будет установлена ошибка
#                        Версия будет помещена в стек
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

    DetailPrint "Ошибка открытия файла $OutDir\version.txt"
    goto error

  no:

    FileRead $0 $1 # Номер версии
    FileRead $0 $1 # находится на
    FileRead $0 $1 # третей строчке
    ${WordFind} $1 "," "+01" $R0
    DetailPrint "Версия СУБД PostgreSQL: $R0"
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
# Функция окна, предоставляющего пользователю выбор папки установки БД САВОИ
#------------------------------------------------------------------------------
!macro SetDir4DataPage
Function SetDir4DataPage

  # Скрывать страницу следует тогда, когда в устанавливаемые компоненты не входит
  # установка PostgreSQL
  SectionGetFlags ${secPostgreSQL} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} equal less more
  less:
  more:
    Abort
  equal:
  
  # Скрывать страницу следует тогда, когда PostgreSQL установлена
  ClearErrors
  ReadRegStr $0 HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes1 no1
  no1: # PostgreSQL установлена
    goto InstallNotRequired # Отображаем окно, где говорится о том, что
                            # postgresql-x64-9.1 установлена
  yes1: # PostgreSQL НЕ установлена
                          
  !insertmacro MUI_HEADER_TEXT "Установка директории, где будут хранится данные \
                                БД" ""

  var /GLOBAL Dialog
  var /GLOBAL Label
  var /GLOBAL DirRequest
  var /GLOBAL BrowseButton

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateLabel} 0 0 100% 14u "Выберите директорию, где будут хранится \
                                   данные БД ПО $\"${_MODULE_FULL_NAME_}$\"."
  Pop $Label
	
  ${NSD_CreateLabel} 0 14u 100% 28u "ВНИМАНИЕ: не желательно, чтобы директория \
                                     находилась в папке C:\Program Files\ или \
                                     C:\Program Files(x86)\."
  Pop $Label
  
  ${NSD_CreateGroupBox} 0 42u 100% 35u "Папка установки"
  Pop $Label
	
  StrCmp $DirRequest_State "" yes no
  yes:
    StrCpy $DirRequest_State ${DirRequest_State_Default} # Каталог для установки
                                                         # по умолчанию
  no:
  ${NSD_CreateDirRequest} 7u 56u -70u 14u $DirRequest_State
  Pop $DirRequest
  ${NSD_SetFocus} $DirRequest
	
  ${NSD_CreateBrowseButton} -60u 56u 53u 14u "Обзор ..." # 53u считается с конца
                                                         # т.к. стоит -60u
  Pop $BrowseButton
  ${NSD_OnClick} $BrowseButton OnClick_BrowseButton
  
  goto showDialog
  
  # Отображаем окно, где говорится о том, что
  # postgresql-x64-9.1 установлена
  InstallNotRequired:
  
    !insertmacro MUI_HEADER_TEXT "СУБД PostgreSQL 9.3.3-1 \
                                  уже установлен." ""

    nsDialogs::Create 1018
    Pop $Dialog

    ${If} $Dialog == error
      Abort
    ${EndIf}

    ${NSD_CreateLabel} 0 0 100% -13u "СУБД PostgreSQL 9.3.3-1 уже установлен в системе. \
                                      Будет осуществлена проверка на установленную версию \
                                      и произведен запуск СУБД PostgreSQL 9.3.3-1."
    Pop $Label
  
  goto showDialog
  
  showDialog:

  nsDialogs::Show

FunctionEnd
#------------------------------------------------------------------------------
Function OnClick_BrowseButton

  nsDialogs::SelectFolderDialog /NOUNLOAD \
    "Выберите директория для установки ..." $DirRequest_State
  Pop $0
  ${If} $0 == "error" # Пользователь отменил выбор папки
    Return
  ${EndIf}
  ${If} $0 != ""
    system::Call `user32::SetWindowText(i $DirRequest, t "$0")`
  ${EndIf}
  
FunctionEnd
#------------------------------------------------------------------------------
Function leaveSetDir4DataPage

  # Запоминаем выбранный каталог с целью его восстановления в случае,
  # если пользователь нажмет кнопку "Назад"
  ${NSD_GetText} $DirRequest $DirRequest_State

FunctionEnd
!macroend
