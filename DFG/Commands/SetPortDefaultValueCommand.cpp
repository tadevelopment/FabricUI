// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/Util/RTValUtil.h>
#include "SetPortDefaultValueCommand.h"
#include <FabricUI/Commands/CommandException.h>
#include <FabricUI/Commands/KLCommandManager.h>

using namespace FabricUI;
using namespace DFG;
using namespace Commands;
using namespace FabricCore;

SetPortDefaultValueCommand::SetPortDefaultValueCommand() 
  : BaseDFGCommand()
{
  try
  {
    // Optional arg of known KL type, 
    declareRTValArg(
      "execPath", 
      "String", 
      false);
    
    declareRTValArg(
      "nodeName", 
      "String", 
      false);
    
    declareRTValArg(
      "portName", 
      "String", 
      false);

    // No-optional arg of unknown KL type, which
    // is retrieved when executing the command.
    declareArg("portValue", false);

    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      CommandManager::GetCommandManager());
    
    // Optional arg of known KL type, with default value.                  
    declareRTValArg( 
      "isUndoable",
      "Boolean",
      true,
      RTVal::ConstructBoolean(
        manager->getClient(), 
        true)
      );
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "SetPortDefaultValueCommand::SetPortDefaultValueCommand",
      "",  
      e.getDesc_cstr());
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "SetPortDefaultValueCommand::SetPortDefaultValueCommand",
      "",
      e.what());
  }
}

SetPortDefaultValueCommand::~SetPortDefaultValueCommand() 
{
}

bool SetPortDefaultValueCommand::canUndo()
{
  try 
  {
    return getRTValArg(
      "isUndoable"
      ).getBoolean();
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "SetPortDefaultValueCommand::canUndo",
      "",
      e.what());
  }

  return false;
}

bool SetPortDefaultValueCommand::doIt()
{
  try 
  {
    QString execPath = getRTValArg(
      "execPath"
      ).getStringCString();

    QString nodeName = getRTValArg(
      "nodeName"
      ).getStringCString();

    QString portName = getRTValArg(
      "portName"
      ).getStringCString();

    QString portPath = nodeName + "." + portName;

    DFGExec exec = m_dfgController->getBinding().getExec().getSubExec(
      execPath.toUtf8().constData()
      );

    DFGExec nodeExec = exec.getSubExec(
      nodeName.toUtf8().constData()
      );

    RTVal portVal = getRTValArg(
      "portValue",
      nodeExec.getPortTypeSpec(
        portName.toUtf8().constData())
      );

    bool isUndoable = getRTValArg(
      "isUndoable"
      ).getBoolean();

    exec.setPortDefaultValue( 
    	portPath.toUtf8().constData(), 
    	portVal, 
    	isUndoable);

    if(isUndoable)
		  ++m_coreCmdCount;

    return true;
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "SetPortDefaultValueCommand::doIt",
      "",
      e.what());
  }

  return false;
} 

QString SetPortDefaultValueCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["execPath"] = "Absolute path of the DFGExec";
  argsHelp["nodeName"] = "Name of the node owning the port";
  argsHelp["portName"] = "Name of the port.";
  argsHelp["portValue"] = "Value to set, must be of the same type than the port.";
  argsHelp["isUndoable"] = "If true, the command is undoable.";

  return createHelpFromArgs(
    "Sets the value of a DFG port",
    argsHelp);
}

QString SetPortDefaultValueCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["execPath"] = getRTValArg(
    "execPath").getStringCString();

  argsDesc["nodeName"] = getRTValArg(
    "nodeName").getStringCString();

  argsDesc["portName"] = getRTValArg(
    "portName").getStringCString();

  return createHistoryDescFromArgs(
    argsDesc);
}
