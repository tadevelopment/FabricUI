/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_DESCRIPTION_H__
#define __UI_SCENEHUB_DESCRIPTION_H__

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
 
namespace FabricUI {
namespace SceneHub {

class SHCmdDescription {

  public:
    SHCmdDescription() {}

    ~SHCmdDescription() {}

    QString cmdName;

    QString cmdType;
};

inline bool operator==(const SHCmdDescription &desc1, const SHCmdDescription &desc2) {
  return desc1.cmdName == desc2.cmdName && desc1.cmdType == desc2.cmdType;
}

inline uint qHash(const SHCmdDescription &desc) {
  return qHash(desc.cmdName + "_" + desc.cmdType);
}

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_DESCRIPTION_H__
