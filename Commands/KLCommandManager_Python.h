//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_MANAGER_PYTHON__
#define __UI_KL_COMMAND_MANAGER_PYTHON__

#include <QMap>
#include "BaseCommand.h"
#include "KLCommandManager.h"

namespace FabricUI {
namespace Commands {

class KLCommandManager_Python : public KLCommandManager
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    KLCommandManager_Python "wraps" methods of KLCommandManager throwing C++ exceptions called from Python.
    The exceptions are catched and returned as strings ao they can be raised in Python.   

    In addition, it 'redirects' methods of KLCommandManager that expose the 'Command' interfaces because C++ 
    interfaces cannot be wrapped in pyhton by shiboken.
  */
  Q_OBJECT
  
 	public:
    KLCommandManager_Python(
      FabricCore::Client client
      );

    virtual ~KLCommandManager_Python();

  protected:
    /// Gets the total number of commands
    /// (low+top) at the current index.
    /// Used by the python command manager.
    int _totalCountAtStackIndex();

    /// Wraps CommandManager::doCommand.
    QString _doCommand_Python(
    	BaseCommand *cmd
      );

    /// Wraps CommandManager::undoCommand.
    QString _undoCommand_Python();

    /// Wraps CommandManager::redoCommand.
    QString _redoCommand_Python();

    /// Wraps CommandManager::checkCommandArgs.
    QString _checkCommandArgs_Python(
      BaseCommand *cmd,
      const QMap<QString, QString> &args
      );

    /// Wraps CommandManager::checkCommandArgs.
    QString _checkRTValCommandArgs_Python(
      BaseCommand *cmd,
      const QMap<QString, FabricCore::RTVal> &args
      );

    /// Wraps and redirects CommandManager::createCommand.
    /// Do not call, throws an exception, to override in Python
    virtual BaseCommand* _createCommand_Python(
      const QString &cmdName, 
      const QMap<QString, QString> &args, 
      bool doCmd
      );

    /// Wraps and redirects CommandManager::createRTValCommand.
    /// Do not call, throws an exception, to override in Python
    virtual BaseCommand* _createRTValCommand_Python(
      const QString &cmdName, 
      const QMap<QString, FabricCore::RTVal> &args, 
      bool doCmd
      );

    /// Wraps and redirects CommandManager::commandIsPushed.
    /// Do not call, throws an exception, to override in Python
    virtual void _commandIsPushed_Python(
      BaseCommand *cmd,
      bool isLowCmd = false
      );

  signals:
    /// Wraps and redirects CommandManager::commandPushed signal.
    void _commandPushed_Python(
      BaseCommand *cmd
      );
      
  private slots:
    /// Wraps CommandManager method.
    void onCommandPushed(
      Command *cmd
      );

  private:
    /// Implementation of CommandManager, 
    /// Do not call, throws an exception.
    virtual void doCommand(
      Command *cmd
      );

    /// Implementation of CommandManager, 
    /// Do not call, throws an exception.
    virtual void undoCommand();

    /// Implementation of CommandManager, 
    /// Do not call, throws an exception.
    virtual void redoCommand();

    /// Implementation of CommandManager, 
    /// calls the Python version.
    virtual void checkCommandArgs(
      Command *cmd,
      const QMap<QString, QString> &args
      );

    /// Implementation of RTValCommandManager, 
    /// calls the Python version.
    virtual void checkRTValCommandArgs(
      Command *cmd,
      const QMap<QString, FabricCore::RTVal> &args
      );

    /// Implementation of CommandManager, 
    /// calls the Python version.
    virtual Command* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args, 
      bool doCmd
      );

    /// Implementation of RTValCommandManager, 
    /// calls the Python version.
    virtual Command* createRTValCommand(
      const QString &cmdName, 
      const QMap<QString, FabricCore::RTVal> &args, 
      bool doCmd
      );

    /// Implementation of CommandManager, 
    /// calls the Python version.
    virtual void commandIsPushed(
      Command *cmd,
      bool isLowCmd = false
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_MANAGER_PYTHON__
