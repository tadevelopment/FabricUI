// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortsDefaultValuesCommand.h"
#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
    
SetPortsDefaultValuesCommand::SetPortsDefaultValuesCommand() 
  : BaseDFGCommand()
{
	declareArg(
    "execPath",
    "String",
    false);

  declareArg(
    "nodeName",
    "String",
    false);

  declareArg(
    "portNameList",
    "String[]",
    false);

 	declareArg(
    "portValueList",
    "RTVal[]",
    false);

  Client client = 
    Commands::CommandManager::GetCommandManager()->getFabricClient();
            
  declareArg( 
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
  return getArgAsRTVal(
    "isUndoable"
    ).getBoolean();
}

bool SetPortsDefaultValuesCommand::doIt()
{
  try 
  {
    QString execPath = getArgAsRTVal(
      "execPath"
      ).getStringCString();

    QString nodeName = getArgAsRTVal(
      "nodeName"
      ).getStringCString();

    bool isUndoable = getArgAsRTVal(
      "isUndoable"
      ).getBoolean();

    RTVal portNameList = getArgAsRTVal(
      "portNameList"
      );

    RTVal portValueList = getArgAsRTVal(
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

      // Sets the type of the arg here because 
      // we did not know the RTVal type before.
      setArgType(
        "portValue", 
        nodeExec.getPortTypeSpec(
          portName.toUtf8().constData()
        ));

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
