#ifndef ICEFINDOBJECTPROCDEF_H
#define ICEFINDOBJECTPROCDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEFINDOBJECTPROC_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEFINDOBJECTPROC_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEFINDOBJECTPROC_DECL_EXPORT
#   endif
#endif

#ifndef ICEFINDOBJECTPROC_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEFINDOBJECTPROC_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEFINDOBJECTPROC_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEFINDOBJECTPROC_DLL
#   define ICEFINDOBJECTPROC_EXPORT ICEFINDOBJECTPROC_DECL_EXPORT
#else
#   define ICEFINDOBJECTPROC_EXPORT ICEFINDOBJECTPROC_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEFINDOBJECTPROCDEF_H
