// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortDefaultValueCommand.h"
#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
    
SetPortDefaultValueCommand::SetPortDefaultValueCommand() 
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
    "portName",
    "String",
    false);

 	declareArg(
    "portValue",
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

SetPortDefaultValueCommand::~SetPortDefaultValueCommand() 
{
}

bool SetPortDefaultValueCommand::canUndo()
{
  return getArgAsRTVal(
    "isUndoable"
    ).getBoolean();
}

bool SetPortDefaultValueCommand::doIt()
{
  try 
  {
    QString execPath = getArgAsRTVal(
      "execPath"
      ).getStringCString();

    QString nodeName = getArgAsRTVal(
      "nodeName"
      ).getStringCString();

    QString portName = getArgAsRTVal(
      "portName"
      ).getStringCString();

    QString portPath = nodeName + "." + portName;

    DFGExec exec = m_dfgController->getBinding().getExec().getSubExec(
      execPath.toUtf8().constData()
      );

    DFGExec nodeExec = exec.getSubExec(
      nodeName.toUtf8().constData()
      );
 
    // Sets the type of the arg here because 
    // we did not know the RTVal type before.
    setArgType(
      "portValue", 
      nodeExec.getPortTypeSpec(
        portName.toUtf8().constData()
      ));
 
    RTVal portVal = getArgAsRTVal(
      "portValue"
      );

    bool isUndoable = getArgAsRTVal(
      "isUndoable"
      ).getBoolean();

    exec.setPortDefaultValue( 
    	portPath.toUtf8().constData(), 
    	portVal, 
    	isUndoable
      );

    if(isUndoable)
		  ++m_coreCmdCount;
 
    return true;
  }

  catch(Exception &e)
  {
    std::cout << "SetPortDefaultValueCommand::doIt: exception: " << e.getDesc_cstr() << std::endl;

    printf(
      "SetPortDefaultValueCommand::doIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
} 

QString SetPortDefaultValueCommand::getHelp()
{
  QString help;
  help +=  "Sets the value of a DFG port.\n";
  help +=  "Arguments:\n";
  help +=  "- execPath (String): Absolute path of the DFGExec.\n";
  help +=  "- nodeName (String): Name of the node owning the port.\n";
  help +=  "- portName (String): Name of the port.\n";
  help +=  "- portValue (RTVal): Value to set, must be of the same that the port type.\n";
  help +=  "- isUndoable (Boolean): If true, the command is undoable.";
  return help;
}
