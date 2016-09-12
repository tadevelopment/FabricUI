//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "FabricResourcePath.h"

#include <FTL/Path.h>
#include <stdio.h>
#include <string>

FTL::StrRef FabricResourcesDirPath()
{
  static bool haveResult = false;
  static std::string result;
  if ( !haveResult )
  {
    haveResult = true;

    char const *fabricDir = getenv( "FABRIC_DIR" );
    if ( !fabricDir || !fabricDir[0] )
    {
      fprintf(
        stderr,
        "VETreeWidget::reloadStyles: missing FABRIC_DIR environment variable\n"
        );
    }
    else
    {
      result = fabricDir;
      FTL::PathAppendEntry( result, "Resources" );
    }
  }
  return result;
}
