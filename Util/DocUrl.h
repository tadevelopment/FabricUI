/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRICUI_UTIL_DOCURL_H
#define _FABRICUI_UTIL_DOCURL_H

#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QString>

namespace FabricUI
{
  namespace Util
  {
    class DocUrl
    {
    public:

      // opens an url via QDesktopServices::openUrl().
      // if useLocalFabricDoc is true then the function will try
      // to determine if url is a "Fabric link" and use the local
      // documentation instead of the online one.
      static void openUrl(QString url, bool useLocalFabricDoc = false);

      // returns true if the environment variable envVar is set.
      static bool useLocalDoc(QString envVar = "FABRIC_USE_LOCAL_DOCUMENTATION");
    };
  }
}

#endif //_FABRICUI_UTIL_DOCURL_H
