// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortsDefaultValuesCommand.h"
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Commands/CommandArgFlags.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandArgHelpers.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
 
SetPortsDefaultValuesCommand::SetPortsDefaultValuesCommand() 
  : BaseRTValScriptableCommand()
{
  try
  {
    declareRTValArg("pathValues", "PathValue", CommandArgFlags::LOGGABLE_ARG | CommandArgFlags::IO_ARG);
    
    // No-optional arg of unknown KL type, which
    // is retrieved when executing the command.
    declareArg("newValues");

    // Optional arg of unknown KL type, which
    // is retrieved when executing the command.
    declareArg("previousValues", CommandArgFlags::OPTIONAL_ARG);

    KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
      CommandManager::GetCommandManager());
    
    // Optional arg of known KL type, with default value.                  
    declareRTValArg( 
      "isUndoable",
      "Boolean",
      CommandArgFlags::OPTIONAL_ARG | CommandArgFlags::LOGGABLE_ARG,
      RTVal::ConstructBoolean(
        manager->getClient(), 
        true)
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortsDefaultValuesCommand::SetPortsDefaultValuesCommand",
      "",
      e.what());
  }
}

SetPortsDefaultValuesCommand::~SetPortsDefaultValuesCommand() 
{
}

bool SetPortsDefaultValuesCommand::canUndo()
{
  try 
  {
    return getRTValArg("isUndoable").getBoolean();
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortsDefaultValuesCommand::canUndo",
      "",
      e.what());
  }

  return false;
}

bool SetPortsDefaultValuesCommand::doIt()
{
	// try
 //  {
 //  	KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
 //    	CommandManager::GetCommandManager());
    
 //    RTVal pathValues = getRTValArg("pathValues");

 //    if(canUndo() && !isArgSet("previousValues"))
 //    {
 //    	RTVal previousValues = RTVal::ConstructVariableArray(
 //    		manager->getClient(), "RTVal");

 //    	previousValues.setArraySize(pathValues.getArraySize());

 //    	for(unsigned int i=0; i<pathValues.getArraySize(); ++i)
 //    	{
 //    		RTVal pathValue = pathValues.getArrayElement(i);
 //    		previousValues.setArrayElement(i, pathValue.maybeGetMember("value"));
 //    	}

 //      setRTValArg("previousValues", previousValues);
 //    }
 
 // 		// For the moment, assume that all the port share the same type.
 //    QString portType = PathResolver::GetPathResolver()->getType(pathValues.getArrayElement(0));
 //    RTVal newValues = getRTValArg("newValues", portType+"[]");

 //  	for(unsigned int i=0; i<pathValues.getArraySize(); ++i)
 //  	{
 //  		RTVal pathValue = pathValues.getArrayElement(i);
 //    	pathValue.setMember("value", newValues.getArrayElement(i));
 //  		pathValues.setArrayElement(i, pathValue);
 //  	}
    	
 //    setRTValArg("pathValues", pathValues);

 //    return true;
 //  }

 //  catch(FabricException &e) 
 //  {
 //    FabricException::Throw(
 //      "SetPortsDefaultValuesCommand::doIt",
 //      "",
 //      e.what());
 //  }

  return false;
} 

bool SetPortsDefaultValuesCommand::undoIt()
{ 
  try
  {
    RTVal pathValues = getRTValArg("pathValues");
    RTVal previousValues = getRTValArg("previousValues");

    for(unsigned int i=0; i<pathValues.getArraySize(); ++i)
    {
      RTVal pathValue = pathValues.getArrayElement(i);
      pathValue.setMember("value", previousValues.getArrayElement(i));
      pathValues.setArrayElement(i, pathValue);
    }
      
    setRTValArg("pathValues", pathValues);
    
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortsDefaultValuesCommand::undoIt",
      "",
      e.what());
  }
 
  return false;
} 

bool SetPortsDefaultValuesCommand::redoIt()
{
  return doIt();
} 

QString SetPortsDefaultValuesCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["pathValues"] = "Array of absolute paths of the ports.";
  argsHelp["newValues"] = "Array of new values, must be of the same type than the port.";
  argsHelp["previousValues"] = "Array of previous values, must be of the same type than the port.";
  argsHelp["isUndoable"] = "If true, the command is undoable and loggable.";

  return CreateHelpFromRTValArgs(
    "Sets the values of several DFG ports",
    argsHelp,
    this);
}

QString SetPortsDefaultValuesCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;
 
  RTVal pathValues = getRTValArg("pathValues");

  QString desc = "[";
  for(unsigned int i=0; i<pathValues.getArraySize(); ++i)
	{
		RTVal pathValue = pathValues.getArrayElement(i);
		desc += pathValue.maybeGetMember("value").getStringCString();
		if(i < pathValues.getArraySize() - 1)
			desc += ", ";
	}
  desc += "]";

  argsDesc["pathValues"] = desc;
 
  return CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
