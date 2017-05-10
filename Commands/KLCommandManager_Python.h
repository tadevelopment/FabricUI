//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_MANAGER_PYTHON__
#define __UI_KL_COMMAND_MANAGER_PYTHON__

#include <QPair>
#include "BaseCommand.h"
#include "KLCommandManager.h"

namespace FabricUI {
namespace Commands {

class KLCommandManager_Python : public KLCommandManager
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    KLCommandManager_Python "wraps" methods of KLCommandManager throwing C++ exceptions called from Python.
    The exceptions are catched and returned as strings so they can be raised in Python.   

    In addition, KLCommandManager_Python 'redirects' methods of KLCommandManager that expose the 'Command' 
    interfaces because C++ interfaces cannot be wrapped in pyhton by shiboken.
  */
  Q_OBJECT
  
 	public:
    KLCommandManager_Python(
      FabricCore::Client client
      );

    virtual ~KLCommandManager_Python();

  protected:
    // Python -> C++

    /// Gets the total number of commands
    /// (low+top) at the current index.
    /// Used by the python command manager.
    int _totalCountAtStackIndex();

    /// Wraps and redirects CommandManager::createCommand.
    /// Do not call, throws an exception, to override in Python.
    virtual BaseCommand* _createCommand_Python(
      const QString &cmdName, 
      const QMap<QString, QString> &args, 
      bool doCmd
      );
 
    /// Wraps CommandManager::doCommand.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _doCommand_Python(
    	BaseCommand *cmd
      );

    /// Wraps CommandManager::undoCommand.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _undoCommand_Python();

    /// Wraps CommandManager::redoCommand.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _redoCommand_Python();

    /// Wraps CommandManager::getCommandAtIndex.
    /// Propagates the C++ exception in python 
    /// and redirects the C++ call, to override in Python.
    virtual QPair<QString, BaseCommand*> _getCommandAtIndex_Python(
      unsigned index
      );

    /// Wraps CommandManager::checkCommandArgs.
    /// Propagates the C++ exception in python 
    /// and redirects the C++ call, to override in Python.
    virtual QString _checkCommandArgs_Python(
      BaseCommand *cmd,
      const QMap<QString, QString> &args
      );

    /// Wraps and redirects CommandManager::commandPushed.
    /// Redirects the C++ call, to override in Python.
    virtual void _commandPushed_Python(
      BaseCommand *cmd,
      bool isLowCmd = false
      );

    /// Wraps and redirects RTValCommandManager::createCommand.
    /// Redirects the C++ call, to override in Python.
    virtual BaseCommand* _createRTValCommand_Python(
      const QString &cmdName, 
      const QMap<QString, FabricCore::RTVal> &args, 
      bool doCmd
      );

    /// Wraps RTValCommandManager::checkCommandArgs.
    /// Propagates the C++ exception in python 
    /// and redirects the C++ call, to override in Python.
    virtual QString _checkRTValCommandArgs_Python(
      BaseCommand *cmd,
      const QMap<QString, FabricCore::RTVal> &args
      );

    /// Wraps KLCommandManager::synchronizeKL.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _synchronizeKL_Python();

  signals:
    /// Wraps and redirects CommandManager::commandDone signal.
    /// Propagates the C++ signal in python 
    void _commandDone_Python(
      BaseCommand *cmd
      );
      
  private slots:
    /// Wraps CommandManager method.
    void onCommandDone(
      Command *cmd
      );

  private:
    // C++ -> Python

    /// Implementation of CommandManager, 
    /// calls _createCommand_Python.
    virtual Command* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args, 
      bool doCmd
      );

    /// Implementation of CommandManager, 
    /// calls _doCommand_Python.
    virtual void doCommand(
      Command *cmd
      );

    /// Implementation of CommandManager, 
    /// calls _undoCommand_Python.
    virtual void undoCommand();

    /// Implementation of CommandManager, 
    /// calls _redoCommand_Python.
    virtual void redoCommand();

    /// Implementation of CommandManager, 
    /// calls _getCommandAtIndex_Python.
    virtual Command* getCommandAtIndex(
      unsigned index
      );

    /// Implementation of CommandManager, 
    /// calls _checkCommandArgs_Python.
    virtual void checkCommandArgs(
      Command *cmd,
      const QMap<QString, QString> &args
      );

    /// Implementation of CommandManager, 
    /// calls _commandPushed_Python.
    virtual void commandPushed(
      Command *cmd,
      bool isLowCmd = false
      );

    /// Implementation of RTValCommandManager, 
    /// calls _createRTValCommand_Python.
    virtual Command* createCommand(
      const QString &cmdName, 
      const QMap<QString, FabricCore::RTVal> &args, 
      bool doCmd
      );
    
    /// Implementation of RTValCommandManager, 
    /// calls _checkRTValCommandArgs_Python.
    virtual void checkCommandArgs(
      Command *cmd,
      const QMap<QString, FabricCore::RTVal> &args
      );

    /// Implementation of KLCommandManager.
    /// calls _synchronizeKL_Python.
    virtual void synchronizeKL();
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_MANAGER_PYTHON__
