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

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */

  public:
    BaseCommand();

    virtual ~BaseCommand();

    /// Default implementation of Command.
    /// Sets the command name.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      );

    /// Implementation of Command.
    virtual QString getName();

    /// Default implementation of Command, 
    /// returns false.
    virtual bool canUndo();

    /// Default implementation of Command, 
    /// returns false.
    virtual bool doIt();

    /// Default implementation of Command, 
    /// returns false.
    virtual bool undoIt();

    /// Default implementation of Command, 
    /// returns false.
    virtual bool redoIt();
    
    /// Default implementation of Command, 
    /// returns an empty string 
    virtual QString getHelp();

    /// Default implementation of Command, 
    /// returns the commande name. 
    virtual QString getHistoryDesc();

  private:
    /// Name of the command.
    QString m_name;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND__
