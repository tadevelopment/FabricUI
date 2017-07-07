//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CREATE_TOOLS_COMMAND__
#define __UI_CREATE_TOOLS_COMMAND__

#include "ToolsManager.h"
#include <FabricUI/Commands/BaseRTValScriptableCommand.h>

namespace FabricUI {
namespace Tools {

class CreateToolCommand : public FabricUI::Commands::BaseRTValScriptableCommand
{
  Q_OBJECT
  
  public:
    CreateToolCommand();

    /// Implementation of BaseCommand.
    virtual void registrationCallback(
      QString const&name, 
      void *userData
      );

    virtual ~CreateToolCommand();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool canLog();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual QString getHelp();

  private:
    ToolsManager* m_manager;
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_CREATE_TOOLS_COMMAND__
