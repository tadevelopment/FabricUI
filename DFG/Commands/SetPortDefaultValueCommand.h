// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_SETPORTDEFAULTVALUE_COMMAND__
#define __UI_SETPORTDEFAULTVALUE_COMMAND__

#include "BaseDFGCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandManager.h>

namespace FabricUI {
namespace DFG {

class SetPortDefaultValueCommand : public BaseDFGCommand 
{
  public:
    /**
      SetPortDefaultValueCommand sets the value of a DFG port.

      Args:
      - execPath (String): Absolute path of the DFGExec.
      - portName (String): Name of the port.
      - portValue (RTVal): Value to set, must be of the same that the port type.
      - isUndoable (Boolean): If true, the command is undoable.
    */
    
    SetPortDefaultValueCommand() 
      : BaseDFGCommand()
    {
    	declareArg(
        "execPath",
        "String",
        false);

      declareArg(
        "portName",
        "String",
        false);

     	declareArg(
        "portValue",
        false);

      FabricCore::Client client = 
        Commands::CommandManager::GetCommandManager()->getFabricClient();
                
      declareArg( 
        "isUndoable",
        "Boolean",
        true,
        FabricCore::RTVal::ConstructBoolean(
          client,
          true)
        );
    }

    virtual ~SetPortDefaultValueCommand() 
    {
    }

    /// Implementation of BaseCommand.
    bool canUndo()
    {
      return getArgAsRTVal(
        "isUndoable"
        ).getBoolean();
    }
 
    /// Implementation of BaseCommand.
    virtual bool doIt()
    {
      try 
      {
        QString execPath = getArgAsRTVal(
          "execPath"
          ).getStringCString();

        QString portName = getArgAsRTVal(
          "portName"
          ).getStringCString();

        FabricCore::DFGExec subExec = m_dfgController->getBinding().getExec().getSubExec(
          execPath.toUtf8().constData()
          );
        
        // Sets the type of the arg here because 
        // we did not know the RTVal type before.
        setArgType(
          "portValue", 
          subExec.getPortTypeSpec(
            portName.toUtf8().constData()
          ));

        FabricCore::RTVal portVal = getArgAsRTVal(
          "portValue"
          );

        bool isUndoable = getArgAsRTVal(
          "isUndoable"
          ).getBoolean();
  
        subExec.setPortDefaultValue( 
        	portName.toUtf8().constData(), 
        	portVal, 
        	isUndoable
          );

        if(isUndoable)
    		  ++m_coreCmdCount;

        return true;
      }

      catch(FabricCore::Exception &e)
      {
        printf(
          "SetPortDefaultValueCommand::doIt: exception: %s\n", 
          e.getDesc_cstr());
      }

      return false;
    } 

    /// Implementation of BaseScriptableCommand.
    virtual QString getHelp()
    {
      QString help;
      help +=  "Sets the value of a DFG port.\n";
      help +=  "Arguments:\n";
      help +=  "- execPath (String): Absolute path of the DFGExec.\n";
      help +=  "- portName (String): Name of the port.\n";
      help +=  "- portValue (RTVal): Value to set, must be of the same that the port type.\n";
      help +=  "- isUndoable (Boolean): If true, the command is undoable.";
      return help;
    }
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_SETPORTDEFAULTVALUE_COMMAND__