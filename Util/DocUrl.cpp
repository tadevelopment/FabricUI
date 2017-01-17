/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "DocUrl.h"

#include <QStringList>
#include <QUrl>
#include <QDesktopServices>

using namespace FabricUI::Util;

void DocUrl::openUrl(QString url)
{
  // nothing to open?
  if (url.length() <= 0)
    return;

  // memorize original url.
  QString originalUrl = url;

  // is this a "Fabric" online doc?
  if (   url.contains("docs.fabric-engine.com/")
      || url.contains("documentation.fabricengine.com/"))
  {
    char *fabricDir = getenv("FABRIC_DIR");
    QStringList list = url.split("/HTML/");
    if (!!fabricDir && list.length() >= 2)
    {
      // construct local url version.
      url = QString("file:///") + QString(fabricDir) + QString("/Documentation/HTML/") + list[list.length() - 1];

      // for the local version we need to remove "?highlight=".
      list = url.split("?highlight=");
      if (list.length() > 1)
        url = list[0];
    }
  }

  // open url.
  if (!QDesktopServices::openUrl(url))
  {
    // failed, open original url instead.
    if (url != originalUrl)
      QDesktopServices::openUrl(originalUrl);
  }
}
