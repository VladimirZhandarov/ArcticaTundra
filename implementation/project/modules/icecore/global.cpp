#include "global.h"

namespace ICECORE_NS
{

// Типы файлов изображений
const QStringList imageFileSuffix(QStringList() << "*.jpg" << "*.tif" << "*.png" <<
	"*.bmp");

// Префикс для оригинальных снимков, копируемых в каталог отработанных снимков
const QString prefOriginalFile = "orig";

// Значение по умолчанию при не установленном каком-то ресурсе, например каталоге
const QString NotSet = QObject::tr("Не установлен");

}; // namespace ICECORE_NS
