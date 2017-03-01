//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <string>
#include "CommandManager_Python.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

CommandManager_Python::CommandManager_Python(
  Client client) 
  : CommandManager(client) 
{
}

CommandManager_Python::~CommandManager_Python() 
{
}

QString CommandManager_Python::doCommand_Python(
	BaseCommand *cmd) 
{
  QString error;
	try 
	{ 
		CommandManager::doCommand(cmd);
	}  
	catch (std::string &e) 
	{
		error = e.c_str(); 
	}
  return error;
}
 
QString CommandManager_Python::undoCommand_Python() 
{
  QString error;
  try 
  { 
    CommandManager::undoCommand();
  }
  catch (std::string &e) 
  {
    error = e.c_str(); 
	}
  return error;
}
 
QString CommandManager_Python::redoCommand_Python()
{
  QString error;
  try 
	{ 
		CommandManager::redoCommand();
	}	 
	catch (std::string &e) 
	{
		error = e.c_str(); 
	}
  return error;
}
 
QString CommandManager_Python::checkCommandArgs_Python(
  BaseCommand *cmd,
  const QMap<QString, QString> &args)
{
  QString error;
  try 
  { 
    CommandManager::checkCommandArgs(cmd, args);
  }
  catch (std::string &e) 
  {
    error = e.c_str(); 
  }
  return error;
}

int CommandManager_Python::totalCountAtStackIndex()
{
  int cmdCount = 0;
  for(int i=0; i<m_undoStack.size(); ++i)
  {
    StackedCommand stackedCmd = m_undoStack[i];
    cmdCount += 1 + stackedCmd.lowLevelCmds.size();
  }
  return cmdCount;
}
