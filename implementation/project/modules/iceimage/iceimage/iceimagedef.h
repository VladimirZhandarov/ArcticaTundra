#ifndef ICEIMAGEDEF_H
#define ICEIMAGEDEF_H

#include <QtCore/qglobal.h>


#ifndef ICEIMAGE_DECL_EXPORT
#   ifdef Q_OS_WIN
#       define ICEIMAGE_DECL_EXPORT __declspec(dllexport)
#   else
#       define ICEIMAGE_DECL_EXPORT
#   endif
#endif

#ifndef ICEIMAGE_DECL_IMPORT
#   if defined(Q_OS_WIN)
#       define ICEIMAGE_DECL_IMPORT __declspec(dllimport)
#   else
#       define ICEIMAGE_DECL_IMPORT
#   endif
#endif

#ifdef BUILD_ICEIMAGE_DLL
#   define ICEIMAGE_EXPORT ICEIMAGE_DECL_EXPORT
#else
#   define ICEIMAGE_EXPORT ICEIMAGE_DECL_IMPORT
#endif

#pragma warning( disable : 4290 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4505 )

#endif // ICEIMAGEDEF_H
