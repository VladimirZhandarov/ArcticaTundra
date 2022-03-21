#ifndef ICEGUIDEF_H
#define ICEGUIDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEGUI_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEGUI_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEGUI_DECL_EXPORT
#   endif
#endif

#ifndef ICEGUI_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEGUI_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEGUI_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEGUI_DLL
#   define ICEGUI_EXPORT ICEGUI_DECL_EXPORT
#else
#   define ICEGUI_EXPORT ICEGUI_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEGUIDEF_H
