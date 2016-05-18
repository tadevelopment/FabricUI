/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#include "DocUrl.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>

using namespace FabricUI::Util;

void DocUrl::openUrl(QString url, bool useLocalFabricDoc)
{
  if (url.length() <= 0)
    return;

  QString originalUrl = url;
  if (useLocalFabricDoc)
  {
    // is this a "Fabric" online doc?
    if (   url.contains("docs.fabric-engine.com/")
        || url.contains("documentation.fabricengine.com/"))
    {
      char *fabricDir = getenv("FABRIC_DIR");
      QStringList list = url.split("/HTML/");
      if (!!fabricDir && list.length() >= 2)
      {
        // construct local version.
        url = QString(fabricDir) + QString("/Documentation/HTML/") + list[list.length() - 1];
        // for the local version we need to remove "?highlight=".
        list = url.split("?highlight=");
        if (list.length() > 1)
          url = list[0];
      }
    }
  }

  if (!QDesktopServices::openUrl(url))
  {
    if (url != originalUrl)
      QDesktopServices::openUrl(originalUrl);
  }
}

bool DocUrl::useLocalDoc(QString envVar)
{
  if (envVar.isEmpty())
    return false;
  char *var = getenv(envVar.toUtf8().data());
  return (!!var && *var != '\0');
}
