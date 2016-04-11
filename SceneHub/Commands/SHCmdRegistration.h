/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_REGISTRATION_H__
#define __UI_SCENEHUB_CMD_REGISTRATION_H__

#include <QtCore/QSet>
#include "SGAddObjectCmd.h"
#include "SGAddPropertyCmd.h"
#include "SGSetPropertyCmd.h"
#include "SGSetPaintToolAttributeCmd.h"

namespace FabricUI {
namespace SceneHub {

class SHCmdRegistration  {

  public:
    SHCmdRegistration() { 
      SGAddObjectCmd addObjectCmd;
      registerCommand(&addObjectCmd);

      SGAddPropertyCmd addPropertyCmd;
      registerCommand(&addPropertyCmd);

      SGSetPropertyCmd setPropertyCmd;
      registerCommand(&setPropertyCmd);
 
      SGSetPaintToolAttributeCmd setPaintToolAttributeCmd;
      registerCommand(&setPaintToolAttributeCmd);
    }

    ~SHCmdRegistration() {}

    QSet<SHCmdDescription> getCmdDescriptionSet() { return m_cmdDescriptionSet; }

    void registerCommand(SHCmd *cmd) {
      m_cmdDescriptionSet.insert(cmd->registerCommand());
    }

  private:
    QSet<SHCmdDescription> m_cmdDescriptionSet;
};

} // SceneHub
} // FabricUI 

#endif // __UI_SCENEHUB_CMD_REGISTRATION_H__
