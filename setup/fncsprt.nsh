#==============================================================================
# Название:        Общие данные, макросы и функции для скриптов сборки
#                  дистрибутивов
# Авторские права: ---
#==============================================================================
#
# Автор:     $Author: shchav $
# Создан:    2011.11.08
# Изменен:   $Date: 2012-03-06 15:06:30 +0300 (Р’С‚, 06 РјР°СЂ 2012) $
# Платформа: Intel Windows 7 Professional SP1 x64
#
#------------------------------------------------------------------------------
# Примечание:
#------------------------------------------------------------------------------
# Внимание:
#-----------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Подключение вспомогательных библиотек
#------------------------------------------------------------------------------
!include "WordFunc.nsh"
!include "FileFunc.nsh"

!include "x64.nsh"


#------------------------------------------------------------------------------
# Расчитываемые константы
#------------------------------------------------------------------------------
# Полное наименование модуля
!define _MODULE_FULL_NAME_ "${_MODULE_NAME_}-${_SOFT_VERSION_}"
# Наименование файла инсталлятора
!define _INSTALLER_NAME_ "${_MODULE_NAME_FS_}-${_SOFT_VERSION_}.exe"
# Составная часть пути в файловой системе до бинарных файлов модуля после установки
# и настроечных файлов модуля
!define _PATH_FS_ "${_COMPANY_NAME_FS_}\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}"
# Составная часть пути в меню "Стандартные" до ярлыков модуля после установки
!define _PATH_ "${_COMPANY_NAME_}\${_SYSTEM_NAME_}\ПО $\"${_MODULE_FULL_NAME_}$\""


#------------------------------------------------------------------------------
# Настройка скрипта установки
#------------------------------------------------------------------------------
# Метод запаковки файлов внутри инсталлятора
SetCompressor /SOLID lzma
# Основная надпись на странице приветствия инсталятора
Name "ПО $\"${_MODULE_FULL_NAME_}$\""
# Заголовок окна инсталлятора
Caption "Установка ПО $\"${_MODULE_FULL_NAME_}$\""
# Наименование файла инсталлятора
OutFile "${_INSTALLER_NAME_}"
# Каталог установки по умолчанию
InstallDir "$PROGRAMFILES64\${_PATH_FS_}"
# Отображаем протокол действия инсталлятора/деинсталлятора
ShowInstDetails show
ShowUninstDetails show


#------------------------------------------------------------------------------
# Функции и макросы
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# Инициализация инсталятора
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

  # Установка может быть запущена только администратором
  Push 'Только пользователи из группы "Администраторы" \
        имеют права на установку программного обеспечения'
  Call checkUser

  # Определение по параметрам командной строки необходимость проверки ОС
  ${GetOptions} "$R1" "/chos=" $R0
  StrCmp $R0 "no" nochos chos
chos:
  # Установка должна проходит на определенной ОС Windows
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
  # Для проверки версии операционной системы, куда будет ставиться программа
  # Не забудьте перед расскомменированием этого кода в верхнем коде исправить
  # "error" на "0 #error"
  #MessageBox MB_ICONSTOP|MB_OK "ОС: $R0, Тип: $R1, SP: $R2, Платформа: $R3"
  #Goto error

  # Чтобы на целевой машине программа устанавливалась в папку C:\Programm Files,
  # а не в C:\Programm Files(x86)
  ${DisableX64FSRedirection}

  Goto noerrors

  error:

    MessageBox MB_ICONSTOP|MB_OK "${_MODULE_NAME_} может быть установлено только на \
      Windows ${_WINDOWS_NAME_} ${_WINDOWS_TYPE_} ${_WINDOWS_SERVICE_PACK_} \
      (${_WINDOWS_PLATFORM_ARCHITECTURE_} bit)."
    abort

  noerrors:

  # Для доступа к реестру на 64-битной платформе
  SetRegView 64
  
  ClearErrors

FunctionEnd


#------------------------------------------------------------------------------
# Инициализация деинсталятора
#------------------------------------------------------------------------------
Function un.onInit

  # Удаление может быть запущена только администратором
  Push 'Только пользователи из группы "Администраторы" \
        имеют права на удаление программного обеспечения'
  Call un.checkUser
  
  ClearErrors

  # При запуске деинсталятора в режиме silent ему может быть установлен
  # параметр DUS, значение которого определяет удаление настроек пользователя
  # ПО. При /DUS=0 или его отсутствии - настройки пользователя будут удалены,
  # при других значениях настройки пользователя НЕ будут удалены.
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
  
  # Для доступа к реестру на 64-битной платформе
  SetRegView 64
 
  ClearErrors

FunctionEnd


#------------------------------------------------------------------------------
# Функция проверки текущей учетной записи на соответствие "Администратору"
#
# Входящее значение: В стеке должно лежать сообщение, которое отобразиться
#                    в случае, если текущая учетная запись не является
#                    "Администратором"
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
  Abort "Установка будет прервана"
  
  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция удаления настроечных файлов модуля, которые храняться в папке
# /AppData/Roaming/DVG/savoi/savoidmdapp каждого пользователя.
# Создаются они при запуске модуля для каждого пользователя, из под которого
# был запущен этот модуль.
#------------------------------------------------------------------------------
!macro UN.DeleteSettings
Function un.DeleteSettings

  ClearErrors

  # Вычленяем каталог (D:\Users) из профиля пользователя
  # Администратор, т.к. установщик запускается только из под него.
  ${WordFind} $PROFILE "\" "-1{" $R5

  # Поверяем, запущен ли деинсталлятор в режиме silent
  ifSilent yes no
  yes:
    StrCmp $DUS 0 false true
  no:
    MessageBox MB_YESNO "Удалить настройки приложения для всех пользователей \
                         системы?" IDYES true IDNO false
  true:
    # Находим в этом каталоге все каталоги AppData (их будет столько,
    # сколько пользователей запускало программное обеспечение) и обрабатываем
    # его в функции un.delSets
    ClearErrors
    ${Locate} "$R5" "/L=D /M=${_COMPANY_NAME_FS_}" "un.delSets"
  false:
  
  ClearErrors

FunctionEnd
#------------------------------------------------------------------------------
Function un.delSets

  ClearErrors

  RMDir /r "$R9\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" # Удалили настройки модуля
  ifErrors 0 +2
  MessageBox MB_OK "Ошибка удаления пользовательских файлов настроек ПО $\"${_MODULE_FULL_NAME_}$\": \
					$\n $\"$R9\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}$\" \
					$\nДля удаления пользовательских файлов настроек пользователь \ 
					самостоятельно должен удалить данный каталог."
  # В программной системе могут быть и другие модули. Если их нет, то
  # необходимо зачистить все каталоги, которые были созданы для хранения
  # настроек модуля.
  RMDir "$R9\${_SYSTEM_NAME_FS_}"
  ifErrors +2 0
  RMDir "$R9"

  Push $0 # см. help по Locate для функции обратного вызова

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция настройки/удаления меню: "Пуск" для модуля
#------------------------------------------------------------------------------
!macro SetupStartMenu UN
Function ${UN}SetupStartMenu

  ClearErrors

!if "${UN}" != "un."

  # Настройка файлов и каталогов из меню "Пуск"
  CreateDirectory "$SMPROGRAMS\${_PATH_}"
  CreateShortCut "$SMPROGRAMS\${_PATH_}\ПО $\"${_MODULE_FULL_NAME_}$\".lnk" \
                 "$INSTDIR\${_RUNNING_FILE_}" "" "$INSTDIR\${_RUNNING_FILE_}" 0 \
                  SW_SHOWNORMAL ALT|CONTROL|SHIFT|A "ПО $\"${_MODULE_FULL_NAME_}$\""
  CreateShortCut "$SMPROGRAMS\${_PATH_}\Удалить ПО $\"${_MODULE_FULL_NAME_}$\".lnk" \
                 "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 \
                 SW_SHOWNORMAL ALT|CONTROL|SHIFT|U "Удалить ПО $\"${_MODULE_FULL_NAME_}$\""

!else

  # Удаление файлов и каталогов из меню "Пуск"
  RMDir /r "$SMPROGRAMS\${_PATH_}"
  # В программной системе могут быть и другие модули. Если их нет, то
  # необходимо зачистить все ярлыки, которые были созданы для хранения
  # запускных файлов модуля.
  RMDir "$SMPROGRAMS\${_COMPANY_NAME_}\${_SYSTEM_NAME_}"
  ifErrors +2 0
  RMDir "$SMPROGRAMS\${_COMPANY_NAME_}"

!endif

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция настройки/удаления ярлыка программы на рабочем столе
#------------------------------------------------------------------------------
!macro SetupDesktop UN
Function ${UN}SetupDesktop

  ClearErrors

!if "${UN}" != "un."

  CreateShortCut "$DESKTOP\ПО ${_MODULE_FULL_NAME_}.lnk" \
                 "$INSTDIR\${_RUNNING_FILE_}"
  
!else

  Delete "$DESKTOP\ПО ${_MODULE_FULL_NAME_}.lnk"

!endif
  
  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция регистрации/дерегистрации факта установки модуля в реестре
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

  # Удаляем из реестра регистрацию факта установки модуля
  DeleteRegKey HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}"
  DeleteRegKey /ifempty HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}"
  DeleteRegKey /ifempty HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations"
  DeleteRegKey /ifempty HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\"

!endif

  ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция регистрации/дерегистрации модуля в панели "Установка и удаление
# программ"
#------------------------------------------------------------------------------
!macro RegisterModuleToAddRemoveProgramm UN
Function ${UN}RegisterModuleToAddRemoveProgramm

  ClearErrors

!if "${UN}" != "un."

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${_MODULE_NAME_FS_}" \
    "DisplayName" "ПО $\"${_MODULE_NAME_}$\""
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
# Макрос удаления модуля из установочной папки в файловой системе
#
# Будут удалены все пустые папки, которые расположены выше удаляемой
# установочной папки
#------------------------------------------------------------------------------
!macro DeleteModuleFromFS

  ClearErrors
  
  # Удаление файлов и каталогов из файловой системы, которые были указаны при
  # установке
  RMDir /r $INSTDIR
  ifErrors done 0
  # Удаляем рекурсивно все пустые папки, которые расположены выше
  # установочной папки
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
# Функция сравнения установленной версии модуля с устанавливаемой версии модуля
#
# Вoзвращаемое значение: В стеке будет находится значение:
#                        1. -1 - модуль в системе не установлен;
#                            0 - версии совпадают;
#                            1 - установлена ранняя версия;
#                            2 - установлена более новая версия;
#                        2. Версия установленного модуля
#                          (если модуль не установлен, то "")
#------------------------------------------------------------------------------
Function IsModuleInstalled

  ClearErrors
  ReadRegStr $0 HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
    "Version"
  IfErrors yes no
  yes: # Модуль в системе не установлен
    Push "" # Ложим в стек версию установленного модуля
    Push -1
    goto exit
  no: # Сравнение версий устанавливаемого модуля с устанавливаемым
    Push $0 # Ложим в стек версию установленного модуля
    ${WordFind} $0 "-" "+01" $R0 # 1.52.43-109 берем 1.52.43 у установленной версии
    ${WordFind} ${_SOFT_VERSION_} "-" "+01" $R1 # 1.52.43-109 берем 1.52.43
    ${VersionCompare} "$R0" "$R1" $R2
    IntCmpU $R2 0 equal less more
    equal:
      ${WordFind} $0 "-" "+02" $R0 # 1.52.43-109 берем 109 у установленной версии
      ${WordFind} ${_SOFT_VERSION_} "-" "+02" $R1 # 1.52.43-109 берем 109
      IntCmpU $R0 $R1 equal1 less more1
      equal1:
        Push 0
        DetailPrint "Версии совпадают: $0 ${_SOFT_VERSION_}"
        goto exit
      less:
        Push 1
        DetailPrint "Установлена ранняя версия: $0 ${_SOFT_VERSION_}"
        goto exit
      more1:
        Push 2
        DetailPrint "Установлена более новая версия: $0 ${_SOFT_VERSION_}"
        goto exit
    more:
      IntCmpU $R2 1 more1 less less

    exit:

FunctionEnd


#------------------------------------------------------------------------------
# Функция обработки процесса сравнения устанавливаемой версии модуля с установ
# ленной версии модуля
#
# Входящее значение: 1. В вершине стека должно лежать сообщение, которое
#                       отобразиться в случае, если установленна поздняя
#                       версия модуля
#                    2. Ниже в стеке должно лежать сообщение, которое
#                       отобразиться в случае, если установленна раняя версия
#                       модуля
#                    3. Еще ниже в стеке должно лежать сообщение, которое
#                       отобразиться в случае, если установленна такая же
#                       версия модуля
# При установленной версии модуля и положительном ответе установленная версия
# будет удалена.
#------------------------------------------------------------------------------
!macro HandleCompareVersionModule
Function HandleCompareVersionModule

  ClearErrors
  Call IsModuleInstalled
  Pop $0
  Pop $1 # Забираем из стека установленную версию модуля

  Pop $R3 # Выдаваемае сообщение при установленной поздней версии модуля
  Pop $R2 # Выдаваемае сообщение при установленной ранней версии модуля
  Pop $R1 # Выдаваемае сообщение при совпадении версии модулей

  IntCmp $0 0 0 less more # версии совпадают
    MessageBox MB_YESNO $R1 IDYES reinstall IDNO 0
    Quit
  more:
    IntCmp $0 1 0 less1 more1 # установлена ранняя версия
    MessageBox MB_YESNO $R2 IDYES reinstall IDNO 0
    Quit
    more1:
    less1: # установлена поздняя версия
      MessageBox MB_YESNO $R3 IDYES reinstall IDNO 0
    Quit

    reinstall:
      # Удаляем установленную версию
      ReadRegStr $0 HKLM "SOFTWARE\${_COMPANY_NAME_FS_}\Installations\${_SYSTEM_NAME_FS_}\${_MODULE_NAME_FS_}" \
        "Base Directory"
      MessageBox MB_YESNO "Удалить настройки приложения для всех пользователей \
                           системы?" IDYES true IDNO false
      true:
        ExecWait '"$0\uninstall.exe" /S /DUS=1 /SMS'
        # После запуска деинстраллятора необходимо подождать,
        # пока деинсталлятор закончит работу.
        FP1:
          FindProcDLL::FindProc "Au_.exe"
          StrCmp $R0 "1" FP1 less
      false:
        ExecWait '"$0\uninstall.exe" /S /SMS'
        # После запуска деинстраллятора необходимо подождать,
        # пока деинсталлятор закончит работу.
        FP2:
          FindProcDLL::FindProc "Au_.exe"
          StrCmp $R0 "1" FP2 less

    less:
    
    ClearErrors

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция запуска приложения, если стоит галочка на последней страницы с
# предложением запуска приложения
#------------------------------------------------------------------------------
!macro RunSoft
Function RunSoft

  SetOutPath "$INSTDIR"
  ExecShell open "${_RUNNING_FILE_}"

FunctionEnd
!macroend


#------------------------------------------------------------------------------
# Функция запуска стороннего приложения с выводом потока вывода приложения в
# окно Detailview NSIS. Функция запускает приложение синхронно.
# (Эта функция создавалась для запуска bat-файлов, т.к. при из запуске
# с помощью ExecWait не изменяется возвращаемое значение из bat-файлов,
# всегда 1)
#
# Входящее значение: В стеке должно находится запускаемое стороннее приложение
#
# Вoзвращаемое значение: В $0 будет находится последний вывод стороннего
#                        приложения в поток вывода
#------------------------------------------------------------------------------
!macro RunWait
Function RunWait

  GetFunctionAddress $0 OutputToDetailView #(OutputToDetailView - см. ниже)
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
# Определение макросов
#------------------------------------------------------------------------------
# Запуск инсталлятора/деинсталлятора должен производитьс из под
# учетной записью "Администратора"
!insertmacro checkuser ""
!insertmacro checkuser "un."
