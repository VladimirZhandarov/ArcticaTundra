#ifndef ICEMDIDEF_H
#define ICEMDIDEF_H

#include <QtCore/qglobal.h>

#ifndef ICEMDI_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEMDI_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEMDI_DECL_EXPORT
#   endif
#endif

#ifndef ICEMDI_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEMDI_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEMDI_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEMDI_DLL
#   define ICEMDI_EXPORT ICEMDI_DECL_EXPORT
#else
#   define ICEMDI_EXPORT ICEMDI_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEMDIDEF_H
