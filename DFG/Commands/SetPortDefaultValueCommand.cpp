// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortDefaultValueCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace DFG;
using namespace Commands;

SetPortDefaultValueCommand::SetPortDefaultValueCommand() 
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
    "portName",
    "String",
    false);

 	declareArg(
    "portValue",
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

  catch(Exception &e)
  {
    printf(
      "SetPortDefaultValueCommand::canUndo: exception: %s\n", 
      e.getDesc_cstr());
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
  help +=  "- portValue (RTVal): Value to set, must be of the same type than the port.\n";
  help +=  "- isUndoable (Boolean): If true, the command is undoable.";
  return help;
}
