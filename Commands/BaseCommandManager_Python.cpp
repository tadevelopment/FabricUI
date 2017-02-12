//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <string>
#include "BaseCommandManager_Python.h"

using namespace FabricUI;
using namespace Commands;

BaseCommandManager_Python::BaseCommandManager_Python() 
  : BaseCommandManager() 
{
}

BaseCommandManager_Python::~BaseCommandManager_Python() 
{
}

QString BaseCommandManager_Python::doCommand_Python(
	BaseCommand *cmd) 
{
  QString error;
	try 
	{ 
		BaseCommandManager::doCommand(cmd);
	}  
	catch (std::string &e) 
	{
		error = e.c_str(); 
	}
  return error;
}
 
QString BaseCommandManager_Python::undoCommand_Python() 
{
  QString error;
  try 
  { 
    BaseCommandManager::undoCommand();
  }
  catch (std::string &e) 
  {
    error = e.c_str(); 
	}
  return error;
}
 
QString BaseCommandManager_Python::redoCommand_Python()
{
  QString error;
  try 
	{ 
		BaseCommandManager::redoCommand();
	}	 
	catch (std::string &e) 
	{
		error = e.c_str(); 
	}
  return error;
}
 
QString BaseCommandManager_Python::checkCommandArgs_Python(
  BaseCommand *cmd,
  const QMap<QString, QString> &args) const
{
  QString error;
  try 
  { 
    BaseCommandManager::checkCommandArgs(cmd, args);
  }
  catch (std::string &e) 
  {
    error = e.c_str(); 
  }
  return error;
}

int BaseCommandManager_Python::totalCountAtStackIndex() const
{
  int cmdCount = 0;
  for(int i=0; i<m_undoStack.size(); ++i)
  {
    StackedCommand stackedCmd = m_undoStack[i];
    cmdCount += 1 + stackedCmd.lowLevelCmds.size();
  }
  return cmdCount;
}
