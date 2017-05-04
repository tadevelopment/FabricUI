//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND__
#define __UI_BASE_COMMAND__

#include "Command.h"

namespace FabricUI {
namespace Commands {

class BaseCommand : public Command
{
  /**
    BaseCommand is a default implementation of Command.
    New commands can inheritated this class.

    C++ interfaces cannot be wrapped in pyhton by shiboken.
    If you want your command to be accessible from python,
    it must derived from this class.
  */

  public:
    BaseCommand();

    virtual ~BaseCommand();

    /// Implementation of Command.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      );

    /// Implementation of Command.
    virtual QString getName();

    /// Implementation of Command, false by default.
    virtual bool canUndo();

    /// Implementation of Command, false by default.
    virtual bool doIt();

    /// Implementation of Command, false by default.
    virtual bool undoIt();

    /// Implementation of Command, false by default.
    virtual bool redoIt();
    
    /// Implementation of Command, returns an empty string 
    virtual QString getHelp();

  private:
    /// Name of the command.
    QString m_name;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND__
