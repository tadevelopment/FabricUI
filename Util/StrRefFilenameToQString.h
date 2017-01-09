//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_UTIL_StrRefFilenameToQString_H
#define FABRICUI_UTIL_StrRefFilenameToQString_H

#include <FTL/StrRef.h>
#include <QtCore/QString>

inline QString StrRefFilenameToQString( FTL::StrRef str )
{
#if defined(FTL_PLATFORM_WINDOWS)
  return QString::fromLatin1( str.data(), str.size() );
#else
  return QString::fromUtf8( str.data(), str.size() );
#endif
}

#endif //FABRICUI_UTIL_StrRefFilenameToQString_H
