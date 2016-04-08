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
      m_cmdDescriptionSet.insert(addObjectCmd.registerCommand());

      SGAddPropertyCmd addPropertyCmd;
      m_cmdDescriptionSet.insert(addPropertyCmd.registerCommand());

      SGSetPropertyCmd setPropertyCmd;
      m_cmdDescriptionSet.insert(setPropertyCmd.registerCommand());

      SGSetPaintToolAttributeCmd setPaintToolAttributeCmd;
      m_cmdDescriptionSet.insert(setPaintToolAttributeCmd.registerCommand());
    }

    virtual ~SHCmdRegistration() {}

    QSet<SHCmdDescription> getCmdDescriptionSet() { return m_cmdDescriptionSet; }

  protected:
    QSet<SHCmdDescription> m_cmdDescriptionSet;
};

} // SceneHub
} // FabricUI 

#endif // __UI_SCENEHUB_CMD_REGISTRATION_H__
