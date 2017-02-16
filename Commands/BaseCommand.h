//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND__
#define __UI_BASE_COMMAND__

#include <QString>

namespace FabricUI {
namespace Commands {

class BaseCommand
{
  /**
    BaseCommand defines the functionalities of a command.
    Any command must inherit from this class.
  */

  public:
    BaseCommand();

    virtual ~BaseCommand();

    /// Called when the command is created.
    /// The userData argument is used to pass optional custom data.
    /// The data is referenced by the registery, and given to 
    /// the command with this callback.
    /// To override.
    virtual void registrationCallBack(
      const QString &name, 
      void *userData
      );

    /// Gets the command name.
    const QString &getName() const;

    /// Checks if the command is undoable (false by default).
    /// To override
    virtual bool canUndo() const;

    /// Defines the command logic.
    /// Returns true if succeded, false otherwise.
    /// To override.
    virtual bool doIt();

    /// Defines the undo logic.
    /// Returns true if succeded, false otherwise.
    /// To override.
    virtual bool undoIt();

    /// Defines the redo logic.
    /// Returns true if succeded, false otherwise.
    /// To override.
    virtual bool redoIt();

  protected:
    /// \internal
    QString m_name;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND__
