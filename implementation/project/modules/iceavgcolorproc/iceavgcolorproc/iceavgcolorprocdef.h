#ifndef ICEAVGCOLORPROCDEF_H
#define ICEAVGCOLORPROCDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEAVGCOLORPROC_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEAVGCOLORPROC_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEAVGCOLORPROC_DECL_EXPORT
#   endif
#endif

#ifndef ICEAVGCOLORPROC_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEAVGCOLORPROC_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEAVGCOLORPROC_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEAVGCOLORPROC_DLL
#   define ICEAVGCOLORPROC_EXPORT ICEAVGCOLORPROC_DECL_EXPORT
#else
#   define ICEAVGCOLORPROC_EXPORT ICEAVGCOLORPROC_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEAVGCOLORPROCDEF_H
