#ifndef ICEQWTDEF_H
#define ICEQWTDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEQWT_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEQWT_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEQWT_DECL_EXPORT
#   endif
#endif

#ifndef ICEQWT_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEQWT_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEQWT_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEQWT_DLL
#   define ICEQWT_EXPORT ICEQWT_DECL_EXPORT
#else
#   define ICEQWT_EXPORT ICEQWT_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEQWTDEF_H
