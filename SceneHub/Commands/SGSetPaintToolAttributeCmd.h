/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETPAINTTOOLATTRIBUTECMD_H__
#define __UI_SCENEHUB_SGSETPAINTTOOLATTRIBUTECMD_H__
 
#include "SHCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {

class SGSetPaintToolAttributeCmd : public SHCmd {

  public: 
    SGSetPaintToolAttributeCmd() : SHCmd() {}

    virtual void registerCommand() {
      if(QMetaType::type("SGSetPaintToolAttributeCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetPaintToolAttributeCmd>("SGSetPaintToolAttributeCmd");
    }

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETPAINTTOOLATTRIBUTECMD_H__
