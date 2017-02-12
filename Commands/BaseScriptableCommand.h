//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPABLE_COMMAND__
#define __UI_BASE_SCRIPABLE_COMMAND__

#include <QMap>
#include <QList>
#include "BaseCommand.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand : public BaseCommand
{
  /**
    BaseScriptableCommand defines the methods for commands with arguments.
    Any scriptable command must inherites from this class.

    Because the arguments have to be set from any app/DCCs, they are passed
    as string only. However, we provide helpers methods to convert strings
    to/from commom types.
  */

  public:
    BaseScriptableCommand();
    
    virtual ~BaseScriptableCommand();

    /// Declares and sets an argument.
    /// To call from the command constructor.
    /// Throws an error if the key is empty.
    virtual void declareArg( 
      const QString &key, 
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Gets the arguments.
    QMap<QString, QString> getArgs() const;

    /// Gets the value of an argument.
    virtual QString getArg( 
      const QString &key 
      ) const;

    /// Sets the value of an argument.
    /// Throws an error if the key is empty or hasn't been declared.
    virtual void setArg(
      const QString &key, 
      const QString &value
      );

    /// Validates that all the argments have been correctly set.
    /// Throws an error if not.
    virtual void validateSetArgs() const;

    /// Gets a decription of the arguments.
    virtual QString getArgsDescription() const;

    /// Implementation of Command, returns true.
    virtual bool canUndo() const;

    /// Gets the command help.
    /// To override.
    virtual QString getHelp() const;

  protected:
    /// \internal
    struct ScriptableCommandArgSpec 
    {
      QString defaultValue;
      bool optional;
    };

    /// \internal
    QMap<QString, ScriptableCommandArgSpec> m_argSpecs;
    QMap<QString, QString> m_args;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPABLE_COMMAND__
