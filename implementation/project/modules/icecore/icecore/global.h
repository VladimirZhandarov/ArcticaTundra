#ifndef ICECORE_GLOBAL_H
#define ICECORE_GLOBAL_H 

#include "icecoredef.h"

#include <QtCore/QStringList>

namespace ICECORE_NS
{

// Типы файлов изображений
extern ICECORE_EXPORT const QStringList imageFileSuffix;

// Префикс для оригинальных снимков, копируемых в каталог отработанных снимков
extern ICECORE_EXPORT const QString prefOriginalFile;

// Значение по умолчанию при не установленном каком-то ресурсе, например каталоге
extern ICECORE_EXPORT const QString NotSet;

}; // namespace ICECORE_NS

#endif // ICECORE_GLOBAL_H
