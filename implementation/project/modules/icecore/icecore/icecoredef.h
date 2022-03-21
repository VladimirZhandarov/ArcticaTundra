#ifndef ICECOREDEF_H
#define ICECOREDEF_H

#include <QtCore/qglobal.h>

#ifndef ICECORE_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICECORE_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICECORE_DECL_EXPORT
#   endif
#endif

#ifndef ICECORE_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICECORE_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICECORE_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICECORE_DLL
#   define ICECORE_EXPORT ICECORE_DECL_EXPORT
#else
#   define ICECORE_EXPORT ICECORE_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICECOREDEF_H
