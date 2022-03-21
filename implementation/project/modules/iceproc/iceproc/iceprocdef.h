#ifndef ICEPROCDEF_H
#define ICEPROCDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEPROC_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEPROC_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEPROC_DECL_EXPORT
#   endif
#endif

#ifndef ICEPROC_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEPROC_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEPROC_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEPROC_DLL
#   define ICEPROC_EXPORT ICEPROC_DECL_EXPORT
#else
#   define ICEPROC_EXPORT ICEPROC_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEPROCDEF_H
