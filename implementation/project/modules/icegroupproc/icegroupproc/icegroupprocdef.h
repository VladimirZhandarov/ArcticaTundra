#ifndef ICEGROUPPROCDEF_H
#define ICEGROUPPROCDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEGROUPPROC_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEGROUPPROC_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEGROUPPROC_DECL_EXPORT
#   endif
#endif

#ifndef ICEGROUPPROC_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEGROUPPROC_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEGROUPPROC_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEGROUPPROC_DLL
#   define ICEGROUPPROC_EXPORT ICEGROUPPROC_DECL_EXPORT
#else
#   define ICEGROUPPROC_EXPORT ICEGROUPPROC_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEGROUPPROCDEF_H
