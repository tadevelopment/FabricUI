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
      - execPath (String): Path of the DFGExec.
      - portName (String): Name of the port.
      - portValue (RTVal): Value to set, must be of the same that the port type.
    */
    
    SetPortDefaultValueCommand() 
      : BaseDFGCommand()
    {
    	BaseScriptableCommand::declareArg(
        "execPath",
        false,
        ".");

      BaseScriptableCommand::declareArg(
        "portName",
        false,
        "");

     	BaseScriptableCommand::declareArg(
        "portValue",
        false,
        "");
    }

    virtual ~SetPortDefaultValueCommand() 
    {
    }
 
    /// Implementation of BaseCommand.
    virtual bool doIt()
    {
      try 
      {
        FabricCore::Client client = Commands::CommandManager::GetCommandManager()->getFabricClient();
        
        QString execPath = Util::RTValUtil::jsonToRTVal(
          client,
          getArg("execPath"),
          "String"
          ).getStringCString();

        FabricCore::DFGExec subExec = m_dfgController->getExec().getSubExec(
          execPath.toUtf8().constData());
        
        QString portName = Util::RTValUtil::jsonToRTVal(
          client,
          getArg("portName"),
          "String"
          ).getStringCString();

        QString portType = subExec.getPortTypeSpec(
          portName.toUtf8().constData()
          );
        
        FabricCore::RTVal portVal = Util::RTValUtil::jsonToRTVal(
          client,
          getArg("portValue"),
          portType
          );
 
        subExec.setPortDefaultValue( 
        	portName.toUtf8().constData(), 
        	portVal, 
        	true
          );

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
      help +=  "- execPath (String): Path of the DFGExec.\n";
      help +=  "- portName (String): Name of the port.\n";
      help +=  "- portValue (RTVal): Value to set, must be of the same that the port type.";
      return help;
    }
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_SETPORTDEFAULTVALUE_COMMAND__