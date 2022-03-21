#==============================================================================
# Название:        Скрипт сборки дистрибутива СПО ЛЕД
# Авторские права: ООО "ДВГ" DeveloperGroup (г. Санкт-Петербург)
#==============================================================================
#
# Автор:     $Author: shchav $
# Создан:    2011.11.08
# Изменен:   $Date: 2012-03-06 15:06:30 +0300 (Р’С‚, 06 РјР°СЂ 2012) $
# Модуль:    Colos
# Платформа: Intel Windows 7 Professional SP1
#
#------------------------------------------------------------------------------
# Примечание:
#------------------------------------------------------------------------------
# Внимание:
#-----------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Подключение вспомогательных библиотек
#------------------------------------------------------------------------------
# Подключение "современного" интерфейса
!include "MUI2.nsh"


#------------------------------------------------------------------------------
# Константы модуля
#------------------------------------------------------------------------------
# Наименование модуля
!define _MODULE_NAME_ "ТУНДРА"
!define _MODULE_NAME_FS_ "tundraapp"
!define _RUNNING_FILE_ "tundraapp_d.exe" # Файл, который запуститься
                                      # по окончанию установки


#------------------------------------------------------------------------------
# Подключаемые скрипты
#------------------------------------------------------------------------------
!include "projectfncsprt.nsh"
!include "postgresql.nsh"


#------------------------------------------------------------------------------
# Настройка современного интерфейса
#------------------------------------------------------------------------------
# Некоторые настройки страниц должны происходить до инициализации самих страниц

# Иконка инсталлятора
!define MUI_ICON "..\implementation\project\modules\iceapp\iceapp\images\installicon.ico"

# Иконка деинсталлятора
!define MUI_UNICON "..\implementation\project\modules\iceapp\iceapp\images\uninstallicon.ico"

# Своя картинка в приветствии инсталятора
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\implementation\project\modules\iceapp\iceapp\images\installpage.bmp"

# Своя картинка в приветствии деинсталятора
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\implementation\project\modules\iceapp\iceapp\images\uninstallpage.bmp"

#------------------------------------------------------------------------------
# Страницы, которые будут видны конечному пользователю при инсталляции
#, порядок их перечисления имеет значение

!define MUI_ABORTWARNING # Выдавать предупреждение при закрытии инсталлятора
                         # пользователем, например, "Вы дейтсвительно хотите"
                         # прервать установку" и т.д.

# Страница приветствия
!define MUI_WELCOMEPAGE_TITLE_3LINES # Чтобы приветствие влезало
!insertmacro MUI_PAGE_WELCOME # Страница привествия

# Страница установки компонент
!insertmacro MUI_PAGE_COMPONENTS # Страница установки компонент

# Страница установки пути к папке, где будет создана БД
# (как правило это папка data)
PageEx custom
  PageCallbacks SetDir4DataPage leaveSetDir4DataPage
PageExEnd

# Страница установки директории
!insertmacro MUI_PAGE_DIRECTORY # Страница установки директории

# Страница прогресса выполнения инсталлятора
!insertmacro MUI_PAGE_INSTFILES # Страница прогресса выполнения инсталлятора

# Страница завершения работы установщика
!define MUI_FINISHPAGE_TITLE_3LINES # Чтобы влезала надпись
!define MUI_FINISHPAGE_RUN_TEXT "Запустить ПО $\"${_MODULE_FULL_NAME_}$\""
!define MUI_FINISHPAGE_RUN $INSTDIR\${_RUNNING_FILE_} # Чтобы появилась галочка на последней
                                                      # страницы с предложением запуска
                                                      # приложения
!define MUI_PAGE_CUSTOMFUNCTION_SHOW mui_page_finish_show # Для того, чтобы снять чекбокс запуска
                                                          # программы. Его должен устанавливать
                                                          # пользователь.
!insertmacro MUI_PAGE_FINISH # Страница завершения работы установщика

#------------------------------------------------------------------------------
# Страницы, которые будут видны конечному пользователю при деинсталляции
#, порядок их перечисления имеет значение

!define MUI_UNABORTWARNING # Выдавать предупреждение при закрытии инсталлятора
                           # пользователем, например, "Вы дейтсвительно хотите"
                           # прервать установку" и т.д.

# Страница приветствия деинсталятора
!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_UNPAGE_WELCOME # Страница приветствия деинсталятора

# Страница удаления компонент
!insertmacro MUI_UNPAGE_COMPONENTS # Страница удаления компонент

# Страница подтверждения удаления
!insertmacro MUI_UNPAGE_CONFIRM # Страница подтверждения удаления

# Страница процесса удаления деинсталятора
!insertmacro MUI_UNPAGE_INSTFILES # Страница процесса удаления деинстлятора

# Страница завершения работы деинсталятора
!define MUI_FINISHPAGE_TITLE_3LINES # Чтобы влезала надпись
!insertmacro MUI_UNPAGE_FINISH # Страница завершения работы деинсталятора

!insertmacro MUI_LANGUAGE "Russian" # Все страницы должны быть на русском языке


#------------------------------------------------------------------------------
# Секция установки компонента СПО ТУНДРА
#------------------------------------------------------------------------------
Section "ПО ${_MODULE_NAME_}" secTundraApp

  # Установка производится для всех пользователей
  SetShellVarContext all

  # Определяем версию установленного модуля в системе
  # (если он, конечно установлен)
  Call IsModuleInstalled
  Pop $0
  Pop $1 # Забираем из стека установленную версию модуля

  # Обрабатываем факт наличия/отсутствия установленной версии ПО
  Push "ПО $\"${_MODULE_NAME_}$\" уже установлено. \
        $\nВерсия установленного ПО ($1) совпадает с устанавливаемым (${_SOFT_VERSION_}). \
        $\nПереустановить ПО $\"${_MODULE_NAME_}$\" (${_SOFT_VERSION_})?"
  Push "Установлена более ранняя версия ПО $\"${_MODULE_NAME_}$\" ($1). \
        $\nПереустановить ПО $\"${_MODULE_NAME_}$\" (${_SOFT_VERSION_})?"
  Push "Установлена более новая версия ПО $\"${_MODULE_NAME_}$\" ($1). \
        $\nПереустановить ПО $\"${_MODULE_NAME_}$\" (${_SOFT_VERSION_})?"
  Call HandleCompareVersionModule

  # Зависимость от модулей среды Visual Studio
  SetOutPath "$TEMP\${_COMPANY_NAME_FS_}"
  # Для работы самого приложения в сборке debug
  File "opo\Visual C++ 2008 Redistributable Debug (x64)\mergemoduledebugsetup(x64).msi"
  ExecWait "msiexec.exe /i $\"$TEMP\${_COMPANY_NAME_FS_}\mergemoduledebugsetup(x64).msi$\" /qn /passive \
            ALLUSERS=1"
  RMDir /r "$TEMP\${_COMPANY_NAME_FS_}\"

  SetOutPath "$INSTDIR"
  # Зависимость от модулей проекта
  !define PROJECTDIR "..\implementation\project"
  file "${PROJECTDIR}\*.dll"
  file "${PROJECTDIR}\${_RUNNING_FILE_}"

  # Зависимость от модулей CUDA
  !define CUDA_TOOL55 "$%CUDA_TOOL55%\bin"
  file "${CUDA_TOOL55}\cudart64_55.dll"
  file "${CUDA_TOOL55}\nppi64_55.dll"
  file "${CUDA_TOOL55}\nppc64_55.dll"
  file "${CUDA_TOOL55}\npps64_55.dll"
  file "${CUDA_TOOL55}\cufft64_55.dll"

  # Зависимость от модулей opencv
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

  # Зависимость от библиотеки GDAL
  !define GDAL1111 "$%GDAL1111%"
  file "${GDAL1111}\Debug\bin\gdal111.dll"
  file "${GDAL1111}\Debug\bin\gdal_translate.exe"

  # Зависимость от модуля CODESYN_ODB_PGSQL231
  !define CODESYN_ODB_PGSQL231 "$%CODESYN_ODB_PGSQL231%\bin64"
  file "${CODESYN_ODB_PGSQL231}\odb-pgsql-d-2.3-vc9.dll"

  # Зависимость от модуля CODESYN_ODB_COMMON230
  !define CODESYN_ODB_COMMON230 "$%CODESYN_ODB_COMMON230%\bin64"
  file "${CODESYN_ODB_COMMON230}\odb-d-2.3-vc9.dll"

  # Зависимость от библиотек PostgreSQL 9.3.3-1
  !define PGSQL9331 "$%PGSQL9331%\bin"
  file "${PGSQL9331}\libpq.dll"
  file "${PGSQL9331}\ssleay32.dll"
  file "${PGSQL9331}\libeay32.dll"
  file "${PGSQL9331}\libintl-8.dll"
  #file "${PGSQL9331}\libiconv-2.dll"

  # Зависимость от библиотеки QJSON
  !define QJSON "$%QJSON%"
  file "${QJSON}\src\qjsond.dll"

  #SetOutPath "$INSTDIR\testdata"
  ## Тестовые данные
  #!define TESTDATA "..\..\testdata\"
  #File "${TESTDATA}\IMG_0651.JPG"

  # Справка программы
  #File /r /x ".svn" "${SOIIDIR}\modules\soiikpmsrvapp\materials\doc"

  SetOutPath "$INSTDIR"
  # Зависимость от Qt библиотек
  !define QTDIR473 "$%QTDIR%"
  File "${QTDIR473}\bin\QtCored4.dll"
  File "${QTDIR473}\bin\QtGuid4.dll"
  File "${QTDIR473}\bin\QTSVGD4.DLL"
  File "${QTDIR473}\bin\QTOPENGLD4.DLL"
  File "${QTDIR473}\bin\QtNetworkd4.dll"
  File "${QTDIR473}\bin\QtWebKitd4.dll"
  File "${QTDIR473}\bin\QtXmld4.dll"

  SetOutPath "$INSTDIR"
  # Зависимость от Qwt библиотеки
  !define QWT610 "$%QWT610%"
  File "${QWT610}\lib\qwtd.dll"

  # Конфигурационные файлы по умолчанию
  #!define CONFIGFILE "..\savoi\modules\savoiclntapp\materials\config"
  #file /r "${CONFIGFILE}\*.*"

  # Устанавливаем деинсталлятор
  # Его необходимо установить раньше чем вызов "SetupStartMenu".
  # Иначе иконка для ярлыка деинсталятора не будет такой, какая реально находиться в
  # файле uninstall.exe под индексом 0.
  WriteUninstaller "uninstall.exe"

  # Настройка меню "Пуск"
  Call SetupStartMenu

  # Настройка ярлыка на рабочем столе
  Call SetupDesktop

  # Регистрируем факт установки модуля в реестре
  Call RegisterModule

  # Регистрируем в панели "Установка и удаление
  # программ"
  Call RegisterModuleToAddRemoveProgramm

  # Не закрываем окно автоматически (для анализа выводы процесса инсталляции)
  SetAutoClose false

SectionEnd

#------------------------------------------------------------------------------
# Секция установки PostgreSQL 9.3.3-1
#------------------------------------------------------------------------------
Section "PostgreSQL 9.3.3-1" secPostgreSQL

  # Установка производится для всех пользователей
  SetShellVarContext all

  # Устанавливаем СУБД PostgreSQL 9.3.3-1, если она не стоит в системе
  Call InstallPostgreSQL913
  Var /GLOBAL baseDirPostgreSQL
  Pop $baseDirPostgreSQL
  ifErrors yes2 no2
  yes2:
    MessageBox MB_OK "Ошибка установки СУБД PostgreSQL 9.3.3-1 \
                      $\nУстановка будет прервана."
    Quit
  no2:

  # Запускаем СУБД PostgreSQL если он не запущен
  Call RunPostgreSQL913
  ifErrors yes no
    yes:
      MessageBox MB_OK "Ошибка запуска СУБД PostgreSQL 9.3.3-1 \
                        $\nУстановка будет прервана."
      Quit
    no:

  # Определение версии запущенного СУБД PostgreSQL
  Push $baseDirPostgreSQL
  Call GetVersionPostgreSQL
  Var /GLOBAL Version
  Pop $Version
  ifErrors yes1 no1
  yes1:
    DetailPrint "Ошибка определения версии сервера СУБД PostgreSQL"
    MessageBox MB_OK "Ошибка определения версии сервера СУБД PostgreSQL. \
                      $\n Установка будет прервана."
    Quit
  no1:
    StrCmp $Version " PostgreSQL 9.3.3" 0 +3
    DetailPrint "Версия СУБД PostgreSQL правильная"
    Goto done1
    DetailPrint "Версия СУБД PostgreSQL НЕ правильная"
    MessageBox MB_OK "Версия запущенного сервера СУБД PostgreSQL: $\"$Version$\".\
                      $\nНе соответствует: $\"PostgresSQL 9.3.3-1$\". \
                      $\nУстановка будет прервана."
    Quit
  done1:

  # Не закрываем окно автоматически (для анализа выводы процесса деинсталляции)
  SetAutoClose false

SectionEnd


#------------------------------------------------------------------------------
# Секция установки БД ЛЁД и ее наката
#------------------------------------------------------------------------------
Section "База данных" secDataBase

  # Установка производится для всех пользователей
  SetShellVarContext all

  # Определяем, установлена ли СУБД PostgreSQL 9.3.3-1
  Var /GLOBAL pathPostgreSQL
  ClearErrors
  ReadRegStr $pathPostgreSQL HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes no
  yes: # СУБД PostgreSQL 9.3.3-1 НЕ установлена
    MessageBox MB_OK "Ошибка создания БД ПО $\"${_MODULE_FULL_NAME_}$\" в СУБД PostgreSQL 9.3.3-1. \
                      $\nСУБД PostgreSQL 9.3.3-1 НЕ установлена. \
                      $\nУстановите СУБД PostgreSQL 9.3.3-1. \
                      $\nУстановка будет прервана."
    Quit
  no: # СУБД PostgreSQL 9.3.3-1 установлена

  # Запускаем СУБД PostgreSQL если он не запущен
  Call RunPostgreSQL913
  ifErrors yes6 no6
  yes6:
    MessageBox MB_OK "Ошибка запуска СУБД PostgreSQL 9.3.3-1. \
                      $\nУстановка будет прервана."
    Quit
  no6:

  # Создание БД icedb
  # Определение, существует ли БД ЛЕД в СУБД
  System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PGPASSWORD", "postgres").r0'
  ExecDos::exec /DETAILED 'cmd /C $\"$\"$pathPostgreSQL$\"\bin\psql -Upostgres -dicedb --command "select version()"$\"'
  Pop $R0
  ClearErrors
  IntCmp $R0 0 yes2 no2 no2
  no2: # БД ЛЕД в СУБД отсутствует
    # Создаем БД ЛЕД в СУБД
    DetailPrint "БД icedb в СУБД PostgresSQL 9.3.3-1 НЕТ"
    ExecDos::exec /DETAILED 'cmd /C $\"$\"$pathPostgreSQL$\"\bin\psql -Upostgres --command "create database icedb"$\"'
    Pop $R0
    ClearErrors
    IntCmp $R0 0 yes3 no3 no3
    yes3: # Создание БД ЛЕД в СУБД прошло успешно
      DetailPrint "БД icedb в СУБД PostgresSQL 9.1.3 успешно создана"

      # Создаем структуру БД ЛЕД, накатываем классификаторы, накатываем тестовые данные
      System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("PGSQL9331", "$pathPostgreSQL").r0' # Это нужно для
                                                                                                 # скриптов наката
      !define MODULESICEODB "..\implementation\project\modules\iceodb"
      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb"
      # Скрипты создания структуры БД ЛЕД
      file "${MODULESICEODB}\iceodb\*.sql"

      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb\iceodb"
      # Основной скрипт создания и наката данных БД ЛЕД
      file "${MODULESICEODB}\materials\db\initdbicedefault.bat"
      file "${MODULESICEODB}\materials\db\initdbice.bat"
      file "${MODULESICEODB}\materials\db\dropdbice.bat"
      file "${MODULESICEODB}\materials\db\isexistdbice.bat"
      file "${MODULESICEODB}\materials\db\setdbicetestdata.bat"
      file "${MODULESICEODB}\materials\db\sqls.txt"

      # Скрипты наката исходных данных
      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb\iceodb\data"
      file "${MODULESICEODB}\materials\db\data\*.sql"

      # Запускаем основной скрипт создания и наката данных БД САВОИ
      ClearErrors
      SetOutPath "$TEMP\${_COMPANY_NAME_FS_}\iceodb\iceodb"
      Push "$OUTDIR\initdbicedefault.bat setup"
      Call RunWait
      DetailPrint "Pop $0"
      StrCmp $0 "SUCCESS!!!" yes33 no33
      yes33: # Создание структуры и наката данных БД ЛЕД в СУБД прошло успешно
        DetailPrint "БД icedb в СУБД PostgresSQL 9.3.3-1 успешно установлена"
        RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
        goto done2
      no33: # Ошибка создания структуры и наката данных БД ЛЕД в СУБД
        MessageBox MB_YESNO  "Ошибка создания структуры и наката данных БД ПО $\"${_MODULE_FULL_NAME_}$\" \
                              $\nв СУБД PostgreSQL 9.3.3-1. \
                              $\nВывести отчет об ошибке? \
                              $\n(Отчет необходимо отправить разработчикам для анализа \
                              $\nпо адресу: ${_COMPANY_E_MAIL_})." IDYES yes44 IDNO no44
        yes44:
          ExecShell open error.txt
          Sleep 1000 # Для того, чтобы успела выполниться предыдущая строчка,
                     # она же ведь асинхронно вызывается.
        no44:
          RMDir /r "$TEMP\${_COMPANY_NAME_FS_}"
        goto no3
    no3:
      MessageBox MB_OK "Ошибка создания БД ПО $\"${_MODULE_FULL_NAME_}$\" в СУБД PostgreSQL 9.3.3-1. \
                        $\nУстановка будет прервана."
      Quit
  yes2: # БД САВОИ в СУБД присутствует
    DetailPrint "БД icedb в СУБД PostgresSQL 9.3.3-1 присутствует"
    MessageBox MB_YESNO "БД ПО $\"${_MODULE_FULL_NAME_}$\" в СУБД PostgreSQL 9.3.3-1 присутствует. \
                         $\nПереустановить БД ПО $\"${_MODULE_FULL_NAME_}$\"?" \
                         IDYES yes4 IDNO done2
    yes4: # Удаление БД САВОИ из СУБД
      DetailPrint "Удаление БД icedb в СУБД PostgresSQL 9.3.3-1"
      ExecDos::exec /DETAILED 'cmd /C $\"$\"$pathPostgreSQL$\"\bin\psql -Upostgres --command "drop database icedb"$\"'
      Pop $R0
      ClearErrors
      IntCmp $R0 0 yes5 no5 no5
      yes5: # Удаление БД ЛЕД из СУБД прошло успешно
        DetailPrint "БД icedb в СУБД PostgresSQL 9.3.3-1 успешно удалена"
        goto no2 # Переход на создание БД ЛЕД в СУБД
      no5: # Ошибка удаление БД ЛЕД из СУБД
        MessageBox MB_OK "Ошибка удаления БД ПО $\"${_MODULE_FULL_NAME_}$\" в СУБД PostgreSQL 9.3.3-1. \
                          $\nУстановка будет прервана."
        Quit

  done2:

  # Не закрываем окно автоматически (для анализа выводы процесса деинсталляции)
  SetAutoClose false

SectionEnd


#------------------------------------------------------------------------------
# Секция удаление компонента СПО ТУНДРА
#------------------------------------------------------------------------------
Section "un.${_MODULE_NAME_}" secUNTundraApp

  # Удаление производится для всех пользователей
  SetShellVarContext all

  # Удаление ярлыка на рабочем столе
  Call un.SetupDesktop

  # Удаляем модуль из меню "Пуск"
  Call un.SetupStartMenu

  # Удаляем регистрацию в панели "Установка и удаление
  # программ"
  Call un.RegisterModuleToAddRemoveProgramm

  # Удаляем модуль из установочной папки в файловой системы
  !insertmacro DeleteModuleFromFS

  # Удаляем регистрацию модуля в реестре
  Call un.RegisterModule

  # Не закрываем окно автоматически (для анализа выводы процесса деинсталляции)
  SetAutoClose false

SectionEnd


#------------------------------------------------------------------------------
# Секция удаления PostgreSQL 9.3.3-1
#------------------------------------------------------------------------------
Section /o "un.PostgreSQL 9.3.3-1" secUNPostgreSQL

  # PostgreSQL 9.3.3-1 не должен удаляться в режиме Silent
  ifSilent 0 +2
    return

  # Определяем, установлена ли СУБД PostgreSQL 9.3.3-1 и где
  ClearErrors

  ReadRegStr $pathPostgreSQL HKLM "SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3" "Base Directory"
  ifErrors yes no
  no: # СУБД PostgreSQL 9.3.3-1 установлена и расположена по пути $pathPostgreSQL

    # Определяем путь к каталогу, где храниться БД (чуть ниже мы его удалим)
    # Его необходимо определить сейчас, так как запись о нем в реестре удалиться после удаления СУБД
    ReadRegStr $0 HKLM SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3\ "Data Directory"
    ifErrors 0 +2
        MessageBox MB_OK "Ошибка определения каталога хранения БД ПО $\"${_MODULE_FULL_NAME_}$\" \
                          $\nКаталог удален не будет."

    # Определяем путь к каталогу, где храниться СУБД (чуть ниже мы его удалим)
    # Его необходимо определить сейчас, так как запись о нем в реестре удалиться после удаления СУБД
    # Этот каталог необходимо будет удалить дополнительно, т.к. деинсталлятор СУБД этого иногда не 
    # делает
    ReadRegStr $1 HKLM SOFTWARE\PostgreSQL\Installations\postgresql-x64-9.3\ "Base Directory"
    ifErrors 0 +2
        MessageBox MB_OK "Ошибка определения каталога установки СУБД ПО $\"${_MODULE_FULL_NAME_}$\" \
                          $\nКаталог установки СУБД удален не будет."

    # Удаляем СУБД
    ExecWait "$pathPostgreSQL\uninstall-postgresql.exe --unattendedmodeui minimal \
              --mode unattended"
    Sleep 5000 # Для того, чтобы успела выполниться предыдущая строчка,
               # она же ведь асинхронно вызывается.
    # Удаляем СУБД из файловой системы полностью, если был определен каталог установки СУБД
    StrCmp $1 "" +4 0
    RMDir /r $1"\.." # Удаляемый каталог находиться на уровень выше того, что получим из реестра
    ifErrors iy in
    iy: DetailPrint "Ошибка удаления каталога установки СУБД: $\"$1$\""
        goto id
    in: DetailPrint "Каталог установки СУБД: $\"$1$\" удален успешно"
    id:

    # Удаляем БД
    MessageBox MB_YESNO  "Удалить Базу Данных ПО $\"${_MODULE_FULL_NAME_}$\"? \
                          $\n$\nВНИМАНИЕ: УДАЛЕНИЕ БАЗЫ ДАННЫХ УНИЧТОЖИТ ВСЕ ДАННЫЕ ФИЛЬТРОВ!!!" \
                          IDYES yes55 IDNO yes
    # Удаляем БД, если определен данный каталог (см. выше)
    yes55: StrCmp $0 "" +4 0
           RMDir /r $0
           ifErrors 0 +2
           MessageBox MB_OK "Ошибка удаления каталога хранения БД ПО $\"${_MODULE_FULL_NAME_}$\": \
                             $\n $\"$0$\" \
                             $\nДля полного удаления СУБД PostgreSQL 9.3.3-1 пользователь \ 
                             самостоятельно должен удалить данный каталог."

  yes: # СУБД PostgreSQL 9.3.3-1 НЕ установлена

  ClearErrors
  
SectionEnd


#------------------------------------------------------------------------------
# Предопределенный функции для стандартной страницы вывода папки установки
# Стандартная страница выводит каталог
#------------------------------------------------------------------------------
Function mui_page_directory_pre

  # Скрывать страницу следует тогда, когда в устанавливаемые компоненты не входит
  # установка ПО ТУНДРА, путь то не к чему указывать.
  SectionGetFlags ${secTundraApp} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} equal less more
  less:
  more:
    Abort
  equal:

FunctionEnd


#------------------------------------------------------------------------------
# Предопределенный функции для стандартной последней страницы 
#------------------------------------------------------------------------------
Function mui_page_finish_show

  # По умолчанию чекбокс запуска программы должен быть снят, он должен устанавливаться
  # пользователем.
  SendMessage $mui.FinishPage.Run ${BM_SETCHECK} ${BST_UNCHECKED} 0

  # Для того, чтобы скрыть чекбокс запуска программы, если не устанавливается сам
  # ПО ТУНДРА (не выбран на вкладке компонентов), запускать то нечего.
  SectionGetFlags ${secTundraApp} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} equal less more
  less:
  more:
    ShowWindow $mui.FinishPage.Run ${SW_HIDE}
  equal:

FunctionEnd


#------------------------------------------------------------------------------
# Описание компонентов
#------------------------------------------------------------------------------
LangString DESC_secTundraApp ${LANG_RUSSIAN} "ПО $\"${_MODULE_FULL_NAME_}$\""
LangString DESC_secPostgreSQL ${LANG_RUSSIAN} "СУБД PostgreSQL 9.3.3-1"
LangString DESC_secDataBase ${LANG_RUSSIAN} "БД ПО $\"${_MODULE_FULL_NAME_}$\". \
                                             Разворачивается на \
                                             локальной СУБД PostgreSQL 9.3.3-1 \
                                             используя пользователя postgres c \
                                             паролем postgres"
# Для компонентов инсталлятора
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${secTundraApp} $(DESC_secTundraApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${secPostgreSQL} $(DESC_secPostgreSQL)
  !insertmacro MUI_DESCRIPTION_TEXT ${secDataBase} $(DESC_secDataBase)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

# Для компонентов деинсталлятора
!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${secTundraApp} $(DESC_secTundraApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${secPostgreSQL} $(DESC_secPostgreSQL)
#  !insertmacro MUI_DESCRIPTION_TEXT ${secDataBase} $(DESC_secDataBase)
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END


#------------------------------------------------------------------------------
# Определение макросов
#------------------------------------------------------------------------------
# Удаление настроечных файлов модуля
!insertmacro UN.DeleteSettings

# Регистрация/дерегистрация факта установки модуля в реестре
!insertmacro RegisterModule ""
!insertmacro RegisterModule "un."

# Настройка/удаление меню: "Пуск" для модуля
!insertmacro SetupStartMenu ""
!insertmacro SetupStartMenu "un."

# Настройка/удаление ярлыка на рабочем столе
!insertmacro SetupDesktop ""
!insertmacro SetupDesktop "un."

# Функция регистрации/дерегистрации модуля в панели "Установка и удаление
# программ"
!insertmacro RegisterModuleToAddRemoveProgramm ""
!insertmacro RegisterModuleToAddRemoveProgramm "un."

# Сравнение устанавливаемой версии модуля с установленной версии модуля
!insertmacro HandleCompareVersionModule

# Установка и определения наличия установленной СУБД PostgreSQL 9.3.3-1
!insertmacro InstallPostgreSQL913

# Запуск сервера СУБД PostgreSQL 9.3.3-1 если сервер СУБД PostgreSQL не
# запущен
!insertmacro RunPostgreSQL913

# Получение версии запущенного сервера СУБД PostgreSQL
!insertmacro GetVersionPostgreSQL

# Для запуска стороннего приложения в синхронном режиме и получения
# последнего вывода стороннего приложения в консоль
!insertmacro RunWait

# Функция одноименной страницы (см. выше в этом файле)
# установки пути, куда будет устанавливаться БД ЛЕД
!insertmacro SetDir4DataPage
