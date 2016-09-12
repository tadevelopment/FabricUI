//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_UTIL_FABRIC_RESOURCE_PATH_H
#define FABRICUI_UTIL_FABRIC_RESOURCE_PATH_H

#include <FTL/Path.h>
#include <FTL/StrRef.h>
#include <QtCore/QString>
#include <FabricUI/Util/StrRefFilenameToQString.h>

FTL::StrRef FabricResourcesDirPath();

inline std::string FabricResourcePath( FTL::StrRef base1 )
{
  std::string result = FabricResourcesDirPath();
  FTL::PathAppendEntry( result, base1 );
  return result;
}

inline QString FabricResourcePathQS( FTL::StrRef base1 )
{
  std::string result = FabricResourcePath( base1 );
  return StrRefFilenameToQString( result );
}

inline std::string FabricResourcePath(
  FTL::StrRef base1,
  FTL::StrRef base2
  )
{
  std::string result = FabricResourcesDirPath();
  FTL::PathAppendEntry( result, base1 );
  FTL::PathAppendEntry( result, base2 );
  return result;
}

inline QString FabricResourcePathQS(
  FTL::StrRef base1,
  FTL::StrRef base2
  )
{
  std::string result = FabricResourcePath( base1, base2 );
  return StrRefFilenameToQString( result );
}

inline std::string FabricResourcePath(
  FTL::StrRef base1,
  FTL::StrRef base2,
  FTL::StrRef base3
  )
{
  std::string result = FabricResourcesDirPath();
  FTL::PathAppendEntry( result, base1 );
  FTL::PathAppendEntry( result, base2 );
  FTL::PathAppendEntry( result, base3 );
  return result;
}

inline QString FabricResourcePathQS(
  FTL::StrRef base1,
  FTL::StrRef base2,
  FTL::StrRef base3
  )
{
  std::string result = FabricResourcePath( base1, base2, base3 );
  return StrRefFilenameToQString( result );
}

#endif //FABRICUI_UTIL_FABRIC_RESOURCE_PATH_H
