//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "CommandException.h"
#include "KLCommandManager_Python.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

KLCommandManager_Python::KLCommandManager_Python(
  Client client) 
  : KLCommandManager(client) 
{
  QObject::connect(
    this,
    SIGNAL(commandDone(Command *)),
    this,
    SLOT(onCommandDone(Command *))
    );
}

KLCommandManager_Python::~KLCommandManager_Python() 
{
}

int KLCommandManager_Python::_totalCountAtStackIndex()
{
  int cmdCount = 0;
  for(int i=0; i<m_undoStack.size(); ++i)
  {
    StackedCommand stackedCmd = m_undoStack[i];
    cmdCount += /*top command*/1 + stackedCmd.lowLevelCmds.size();
  }
  return cmdCount;
}

// Python -> C++
// CommandManager
BaseCommand* KLCommandManager_Python::_createCommand_Python(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  CommandException::Throw(
    "KLCommandManager_Python::_createCommand_Python",
    "Method must be overridden",
    "",
    PRINT | THROW);

  return 0;
}

QString KLCommandManager_Python::_doCommand_Python(
	BaseCommand *cmd) 
{
  QString error;

	try 
	{ 
		KLCommandManager::doCommand(cmd);
	}  

  catch (CommandException &e) 
  {
    error = CommandException::Throw(
      "KLCommandManager_Python::_doCommand_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}
 
QString KLCommandManager_Python::_undoCommand_Python() 
{
  QString error;

  try 
  { 
    KLCommandManager::undoCommand();
  }

  catch (CommandException &e) 
  {
    error = CommandException::Throw(
      "KLCommandManager_Python::_undoCommand_Python",
      "",
      e.what(),
      NOTHING);
	}

  return error;
}
 
QString KLCommandManager_Python::_redoCommand_Python()
{
  QString error;

  try 
	{ 
		KLCommandManager::redoCommand();
	}	 

	catch (CommandException &e) 
	{
    error = CommandException::Throw(
      "KLCommandManager_Python::_redoCommand_Python",
      "",
      e.what(),
      NOTHING);
	}

  return error;
}

QPair<QString, BaseCommand*> KLCommandManager_Python::_getCommandAtIndex_Python(
  unsigned index)
{
  QPair<QString, BaseCommand*> pair;

  Command *cmd = KLCommandManager::getCommandAtIndex(
    index);

  BaseCommand* baseCmd = dynamic_cast<BaseCommand*>(
    cmd);

  if(!baseCmd)
    pair.first = CommandException::Throw(
      "KLCommandManager_Python::_getCommandAtIndex_Python",
      "Command '" + cmd->getName() + "' is not a BaseCommand",
      "",
      NOTHING);
  else
    pair.second = baseCmd;

  return pair;
}

QString KLCommandManager_Python::_checkCommandArgs_Python(
  BaseCommand *cmd,
  const QMap<QString, QString> &args)
{
  QString error;

  try 
  { 
    CommandManager::checkCommandArgs(cmd, args);
  }

  catch (CommandException &e) 
  {
    error = CommandException::Throw(
      "KLCommandManager_Python::_checkCommandArgs_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

void KLCommandManager_Python::_commandPushed_Python(
  BaseCommand *cmd,
  bool isLowCmd)
{
  CommandException::Throw(
    "KLCommandManager_Python::_commandPushed_Python",
    "Method must be overridden",
    "",
    NOTHING);
}

// RTValCommandManager
BaseCommand* KLCommandManager_Python::_createRTValCommand_Python(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  CommandException::Throw(
    "KLCommandManager_Python::_createRTValCommand_Python",
    "Method must be overridden",
    "",
    NOTHING);
  
  return 0;
}

QString KLCommandManager_Python::_checkRTValCommandArgs_Python(
  BaseCommand *cmd,
  const QMap<QString, RTVal> &args)
{
  QString error;

  try 
  { 
    KLCommandManager::checkCommandArgs(
      cmd, 
      args);
  }

  catch (CommandException &e) 
  {
    error = CommandException::Throw(
      "KLCommandManager_Python::_checkRTValCommandArgs_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

QString KLCommandManager_Python::_synchronizeKL_Python()
{
  QString error;

  try
  {
    KLCommandManager::synchronizeKL();
  }

  catch (CommandException &e) 
  {
    error = CommandException::Throw(
      "KLCommandManager_Python::_synchronizeKL_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

void KLCommandManager_Python::onCommandDone(
  Command *cmd)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(
    cmd);

  if(!baseCmd)
    CommandException::Throw(
      "KLCommandManager_Python::onCommandDone",
      "Command '" + cmd->getName() + "' is not a BaseCommand",
      "",
      PRINT | THROW);

  emit _commandDone_Python(baseCmd);
}

// C++ -> Python
// CommandManager
Command* KLCommandManager_Python::createCommand(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  return _createCommand_Python(
    cmdName,
    args,
    doCmd);
}

void KLCommandManager_Python::doCommand(
  Command *cmd)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    CommandException::Throw(
      "KLCommandManager_Python::doCommand",
      "Command '" + cmd->getName() + "' is not a BaseCommand",
      "",
      PRINT | THROW);

  _doCommand_Python(
    baseCmd);
}

void KLCommandManager_Python::undoCommand()
{
  _undoCommand_Python();
}

void KLCommandManager_Python::redoCommand()
{
  _redoCommand_Python();
}

Command* KLCommandManager_Python::getCommandAtIndex(
  unsigned index)
{
  QPair<QString, BaseCommand*> pair = _getCommandAtIndex_Python(
    index);
  return pair.second;
}

void KLCommandManager_Python::checkCommandArgs(
  Command *cmd,
  const QMap<QString, QString> &args)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    CommandException::Throw(
      "KLCommandManager_Python::checkCommandArgs",
      "Command '" + cmd->getName()  + "' is not a BaseCommand",
      "",
      PRINT | THROW);

  _checkCommandArgs_Python(
    baseCmd,
    args);
}

void KLCommandManager_Python::commandPushed(
  Command *cmd,
  bool isLowCmd)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    CommandException::Throw(
      "KLCommandManager_Python::commandPushed",
      "Command '" + cmd->getName() + "' is not a BaseCommand",
      "",
      PRINT | THROW);

  _commandPushed_Python(
    baseCmd,
    isLowCmd);
}

// RTValCommandManager
Command* KLCommandManager_Python::createCommand(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  return _createRTValCommand_Python(
    cmdName,
    args,
    doCmd);
}

void KLCommandManager_Python::checkCommandArgs(
  Command *cmd,
  const QMap<QString, RTVal> &args)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
     CommandException::Throw(
      "KLCommandManager_Python::checkCommandArgs",
      "Command '" + cmd->getName()  + "' is not a BaseCommand",
      "",
      PRINT | THROW);

  _checkRTValCommandArgs_Python(
    baseCmd,
    args);
}

void KLCommandManager_Python::synchronizeKL()
{
  _synchronizeKL_Python();
}
