// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortsDefaultValuesCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandException.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
using namespace Commands;

SetPortsDefaultValuesCommand::SetPortsDefaultValuesCommand() 
  : BaseDFGCommand()
{
  try
  {
    declareRTValArg(
      "execPath", 
      "String", 
      false);

    declareRTValArg(
      "nodeName", 
      "String", 
      false);

    declareRTValArg(
      "portNameList", 
      "String[]", 
      false);

   	declareRTValArg(
      "portValueList", 
      false);

    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      CommandManager::GetCommandManager());
                           
    declareRTValArg( 
      "isUndoable",
      "Boolean",
      true,
      RTVal::ConstructBoolean(
        manager->getClient(), 
        true)
      );
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "SetPortDefaultValueCommand::SetPortDefaultValueCommand",
      "",
      e.what());
  }
}

SetPortsDefaultValuesCommand::~SetPortsDefaultValuesCommand() 
{
}

bool SetPortsDefaultValuesCommand::canUndo()
{
  return getRTValArg(
    "isUndoable"
    ).getBoolean();
}

bool SetPortsDefaultValuesCommand::doIt()
{
  try 
  {
    QString execPath = getRTValArg(
      "execPath"
      ).getStringCString();

    QString nodeName = getRTValArg(
      "nodeName"
      ).getStringCString();

    bool isUndoable = getRTValArg(
      "isUndoable"
      ).getBoolean();

    RTVal portNameList = getRTValArg(
      "portNameList");
    
    RTVal portValueList = getRTValArg(
      "portValueList");

    DFGExec exec = m_dfgController->getBinding().getExec().getSubExec(
      execPath.toUtf8().constData()
      );

    DFGExec nodeExec = exec.getSubExec(
      nodeName.toUtf8().constData()
      );

    for(unsigned int i=0; i<portNameList.getArraySize(); ++i)
    {
      QString portName = portNameList.getArrayElement(i).getStringCString();

      QString portPath = nodeName + "." + portName;

      exec.setPortDefaultValue( 
        portPath.toUtf8().constData(), 
        portValueList.getArrayElement(i), 
        isUndoable);

      if(isUndoable)
        ++m_coreCmdCount;
    }

    return true;
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "SetPortsDefaultValuesCommand::doIt",
      "",
      e.what());
  }
  return false;
} 

QString SetPortsDefaultValuesCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["execPath"] = "Absolute path of the DFGExec";
  argsHelp["nodeName"] = "Name of the node owning the port";
  argsHelp["portNameList"] = "Name of the ports.";
  argsHelp["portValueList"] = "Values to set, must be of the same type than the ports.";
  argsHelp["isUndoable"] = "If true, the command is undoable.";

  return createHelpFromArgs(
    "Sets the values of several DFG ports",
    argsHelp);
}

QString SetPortsDefaultValuesCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["execPath"] = getRTValArg(
    "execPath").getStringCString();

  argsDesc["nodeName"] = getRTValArg(
    "nodeName").getStringCString();

  argsDesc["portNameList"] = getRTValArg(
    "portNameList").getStringCString();

  return createHistoryDescFromArgs(
    argsDesc);
}
