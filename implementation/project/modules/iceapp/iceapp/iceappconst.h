// Автор:     $Author: Vladimir Zhandarov $
// Создан:    2014.09.26
// Изменен:   $Date:$
#ifndef ICEAPPCONST_H
#define ICEAPPCONST_H

//------------------------------------------------------------------------------
// Константы приложения
//------------------------------------------------------------------------------

// Наименование организации
static const char* organizationNameStr = "ООО \"ДВГ\"";

// Наименование приложения
static const char* appNameStr = "ПО ТУНДРА";

// Наименование окна справки
static const char* appHelpNameStr = "Справка ПО ТУНДРА";

// Префих сообщения пользователю
static const char* appWarningPrefixStr = "ПО ТУНДРА";

// Домашняя папка пользователя приложения
// Могут храниться INI и LOG-файлы
static const char* homeAppTundra = "/AppData/Roaming/DVG/tundra/tundraapp";
static const char* tempAppIce = "/AppData/Roaming/DVG/ice/iceapp/Temp";

//имя основного файла настроек
static const char* iniSettings = "tundra.ini";

//------------------------------------------------------------------------------
// Иконки приложения
//------------------------------------------------------------------------------
// Иконка приложения
static const char* appIconStr = ":/dvg/iceapp/images/applicationico.png";

#pragma warning(disable : 4290)
#pragma warning(disable : 4127)
#pragma warning(disable : 4505)

#endif // ICEAPPCONST_H
