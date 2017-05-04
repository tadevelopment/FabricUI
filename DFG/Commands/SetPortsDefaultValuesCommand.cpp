// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortsDefaultValuesCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
using namespace Commands;

SetPortsDefaultValuesCommand::SetPortsDefaultValuesCommand() 
  : BaseDFGCommand()
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
    "RTVal[]",
    false);

  KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
    CommandManager::GetCommandManager());
    
  Client client = manager->getClient();
                   
  declareRTValArg( 
    "isUndoable",
    "Boolean",
    true,
    RTVal::ConstructBoolean(
      client,
      true)
    );
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
      "portNameList"
      );
    
    RTVal portValueList = getRTValArg(
      "portValueList"
      );

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
        isUndoable
        );

      if(isUndoable)
        ++m_coreCmdCount;
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "SetPortsDefaultValuesCommand::doIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
} 

QString SetPortsDefaultValuesCommand::getHelp()
{
  QString help;
  help +=  "Sets the values of several DFG ports.\n";
  help +=  "Arguments:\n";
  help +=  "- execPath (String): Absolute path of the DFGExec.\n";
  help +=  "- nodeName (String): Name of the node owning the port.\n";
  help +=  "- portNameList (String[]): Name of the ports.\n";
  help +=  "- portValueList (RTVal[]): Values to set, must be of the same type than the port.\n";
  help +=  "- isUndoable (Boolean): If true, the command is undoable.";
  return help;
}
