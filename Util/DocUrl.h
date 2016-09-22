/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRICUI_UTIL_DOCURL_H
#define _FABRICUI_UTIL_DOCURL_H

#include <stdio.h>
#include <stdlib.h>

#include <QString>

namespace FabricUI
{
  namespace Util
  {
    class DocUrl
    {
    public:

      // opens an url via QDesktopServices::openUrl().
      // this function will first determine if the url is
      // a "Fabric link" and attempt to open the local
      // documentation instead of the online one. In case
      // that fails it will open the url 'as is'.
      static void openUrl(QString url);
    };
  }
}

#endif //_FABRICUI_UTIL_DOCURL_H
