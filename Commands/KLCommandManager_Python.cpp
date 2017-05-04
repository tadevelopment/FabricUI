//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "CommandHelpers.h"
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
    SIGNAL(commandPushed(Command *)),
    this,
    SLOT(onCommandPushed(Command *))
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

QString KLCommandManager_Python::_doCommand_Python(
	BaseCommand *cmd) 
{
  QString error;
	try 
	{ 
		KLCommandManager::doCommand(cmd);
	}  
	catch (std::string &e) 
	{
    printf(
      "KLCommandManager_Python::_doCommand_Python: exception: %s\n", 
      e.c_str());

		error = e.c_str(); 
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
  catch (std::string &e) 
  {
    printf(
      "KLCommandManager_Python::_undoCommand_Python: exception: %s\n", 
      e.c_str());

    error = e.c_str(); 
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
	catch (std::string &e) 
	{
    printf(
      "KLCommandManager_Python::_redoCommand_Python: exception: %s\n", 
      e.c_str());
    
		error = e.c_str(); 
	}
  return error;
}
 
QString KLCommandManager_Python::_checkCommandArgs_Python(
  BaseCommand *cmd,
  const QMap<QString, QString> &args)
{
  QString error;
  try 
  { 
    KLCommandManager::checkCommandArgs(cmd, args);
  }
  catch (std::string &e) 
  {
    printf(
      "KLCommandManager_Python::_checkCommandArgs_Python: exception: %s\n", 
      e.c_str());
    
    error = e.c_str(); 
  }
  return error;
}

QString KLCommandManager_Python::_checkRTValCommandArgs_Python(
  BaseCommand *cmd,
  const QMap<QString, RTVal> &args)
{
  QString error;
  try 
  { 
    KLCommandManager::checkRTValCommandArgs(
      cmd, 
      args);
  }
  catch (std::string &e) 
  {
    printf(
      "KLCommandManager_Python::_checkRTValCommandArgs_Python: exception: %s\n", 
      e.c_str());
    
    error = e.c_str(); 
  }
  return error;
}

BaseCommand* KLCommandManager_Python::_createCommand_Python(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  printAndThrow(
    "KLCommandManager_Python::_createCommand_Python must be overridden");
  return 0;
}

BaseCommand* KLCommandManager_Python::_createRTValCommand_Python(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  printAndThrow(
    "KLCommandManager_Python::_createRTValCommand_Python must be overridden");
  return 0;
}

void KLCommandManager_Python::onCommandPushed(
  Command *cmd)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    printAndThrow(
      "KLCommandManager_Python::onCommandPushed: error, cmd is not a BaseCommand");

  emit _commandPushed_Python(baseCmd);
}

void KLCommandManager_Python::_commandIsPushed_Python(
  BaseCommand *cmd,
  bool isLowCmd)
{
  printAndThrow(
    "KLCommandManager_Python::_commandIsPushed_Python must be overridden");
}

void KLCommandManager_Python::doCommand(
  Command *cmd)
{
  printAndThrow(
    "KLCommandManager_Python::doCommand must not be called");
}

void KLCommandManager_Python::undoCommand()
{
  printAndThrow(
    "KLCommandManager_Python::undoCommand must not be called");
}

void KLCommandManager_Python::redoCommand()
{
  printAndThrow(
    "KLCommandManager_Python::redoCommand must not be called");
}

void KLCommandManager_Python::checkCommandArgs(
  Command *cmd,
  const QMap<QString, QString> &args)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    printAndThrow(
      "KLCommandManager_Python::checkCommandArgs, cmd is not a BaseCommand");

  _checkCommandArgs_Python(
    baseCmd,
    args);
}

void KLCommandManager_Python::checkRTValCommandArgs(
  Command *cmd,
  const QMap<QString, RTVal> &args)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    printAndThrow(
      "KLCommandManager_Python::checkRTValCommandArgs, cmd is not a BaseCommand");

  _checkRTValCommandArgs_Python(
    baseCmd,
    args);
}

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

Command* KLCommandManager_Python::createRTValCommand(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  return _createRTValCommand_Python(
    cmdName,
    args,
    doCmd);
}

void KLCommandManager_Python::commandIsPushed(
  Command *cmd,
  bool isLowCmd)
{
  BaseCommand *baseCmd = dynamic_cast<BaseCommand *>(cmd);

  if(!baseCmd)
    printAndThrow(
      "KLCommandManager_Python::commandIsPushed, cmd is not a BaseCommand");

  _commandIsPushed_Python(
    baseCmd,
    isLowCmd);
}
