//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "KLCommandManager_Python.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

KLCommandManager_Python::KLCommandManager_Python() 
  : KLCommandManager() 
{
  QObject::connect(
    this,
    SIGNAL(commandDone(BaseCommand *)),
    this,
    SLOT(onCommandDone(BaseCommand *))
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
  FabricException::Throw(
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

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
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

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
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

	catch (FabricException &e) 
	{
    error = FabricException::Throw(
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

  pair.second = KLCommandManager::getCommandAtIndex(
    index);

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

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
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
  FabricException::Throw(
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
  FabricException::Throw(
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

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
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

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
      "KLCommandManager_Python::_synchronizeKL_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

void KLCommandManager_Python::onCommandDone(
  BaseCommand *cmd)
{
  
  emit _commandDone_Python(cmd);
}

// C++ -> Python
// CommandManager
BaseCommand* KLCommandManager_Python::createCommand(
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
  BaseCommand *cmd)
{
  _doCommand_Python(
    cmd);
}

void KLCommandManager_Python::undoCommand()
{
  _undoCommand_Python();
}

void KLCommandManager_Python::redoCommand()
{
  _redoCommand_Python();
}

BaseCommand* KLCommandManager_Python::getCommandAtIndex(
  unsigned index)
{
  QPair<QString, BaseCommand*> pair = _getCommandAtIndex_Python(
    index);
  return pair.second;
}

void KLCommandManager_Python::checkCommandArgs(
  BaseCommand *cmd,
  const QMap<QString, QString> &args)
{ 
  _checkCommandArgs_Python(
    cmd,
    args);
}

void KLCommandManager_Python::commandPushed(
  BaseCommand *cmd,
  bool isLowCmd)
{ 
  _commandPushed_Python(
    cmd,
    isLowCmd);
}

// RTValCommandManager
BaseCommand* KLCommandManager_Python::createCommand(
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
  BaseCommand *cmd,
  const QMap<QString, RTVal> &args)
{
  _checkRTValCommandArgs_Python(
    cmd,
    args);
}

void KLCommandManager_Python::synchronizeKL()
{
  _synchronizeKL_Python();
}
