#ifndef ICEAVGLUMPROCDEF_H
#define ICEAVGLUMPROCDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEAVGLUMPROC_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEAVGLUMPROC_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEAVGLUMPROC_DECL_EXPORT
#   endif
#endif

#ifndef ICEAVGLUMPROC_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEAVGLUMPROC_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEAVGLUMPROC_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEAVGLUMPROC_DLL
#   define ICEAVGLUMPROC_EXPORT ICEAVGLUMPROC_DECL_EXPORT
#else
#   define ICEAVGLUMPROC_EXPORT ICEAVGLUMPROC_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEAVGLUMPROCDEF_H
