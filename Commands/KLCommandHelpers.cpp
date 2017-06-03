//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommandHelpers.h"
#include "KLCommandManager.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricCore;

namespace FabricUI {
namespace Commands {

RTVal GetKLCommandManager() 
{
  RTVal klCmdManager;

  try 
  {
    klCmdManager = RTVal::Create(
      Application::FabricApplicationStates::GetAppStates()->getContext(), 
      "AppCommandManager", 
      0, 0);

    klCmdManager = klCmdManager.callMethod(
      "AppCommandManager", 
      "getCommandManager", 
      0, 0);
  } 

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandManager",
      "",
      e.getDesc_cstr());
  }

  return klCmdManager;
}

RTVal GetKLCommandRegistry() 
{
  RTVal klCmdRegistry;

  try 
  {
    klCmdRegistry = RTVal::Construct(
      Application::FabricApplicationStates::GetAppStates()->getContext(), 
      "AppCommandRegistry", 
      0, 0);

    klCmdRegistry = klCmdRegistry.callMethod(
      "AppCommandRegistry", 
      "getCommandRegistry", 
      0, 0);
  } 

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandRegistry",
      "",
      e.getDesc_cstr());
  }

  return klCmdRegistry;
}

QString GetKLCommandName(
  RTVal klCmd) 
{
  try 
  {
    RTVal strVal = klCmd.callMethod(
      "String", 
      "getName", 
      0, 
      0);

    return strVal.getStringCString();
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandName",
      "",
      e.getDesc_cstr());
  }

  return "";
}
 
void SetKLCommandInteractionID(
  FabricCore::RTVal klCmd,
  int interactionID)
{
  try 
  {
    RTVal interactionIDVal = RTVal::ConstructSInt32(
      klCmd.getContext(),
      interactionID);

    klCmd.callMethod(
      "", 
      "setInteractionID", 
      1, 
      &interactionIDVal);
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::SetKLCommandInteractionID",
      "",
      e.getDesc_cstr());
  }
}

int GetKLCommandInteractionID(
  FabricCore::RTVal klCmd)
{
  try 
  {
    return klCmd.callMethod(
      "SInt32", 
      "getInteractionID", 
      0, 
      0).getSInt32();
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandInteractionID",
      "",
      e.getDesc_cstr());
  }
  
  return -1;
}

bool CanKLCommandUndo(
  RTVal klCmd) 
{
  try 
  {
    return klCmd.callMethod(
      "Boolean", 
      "canUndo", 
      0, 
      0).getBoolean();
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::CanKLCommandUndo",
      "",
      e.getDesc_cstr());
  }
  
  return false;
}

bool CanKLCommandLog(
  RTVal klCmd) 
{
  try 
  {
    return klCmd.callMethod(
      "Boolean", 
      "canLog", 
      0, 
      0).getBoolean();
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::CanKLCommandLog",
      "",
      e.getDesc_cstr());
  }
  
  return false;
}

bool DoKLCommand( 
  RTVal klCmd) 
{ 
  QString strError;

  try 
  {
    RTVal args[2] = { 
      klCmd, 
      // error
      RTVal::ConstructString(klCmd.getContext(), "") 
    };

    GetKLCommandManager().callMethod(
      "", 
      "_doCommand", 
      2, 
      args);
    
    strError = args[1].getStringCString();
    if(!strError.isEmpty())
      Application::FabricException::Throw(
        "KLCommandHelpers::DoKLCommand",
        strError);

    return true;
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::DoKLCommand",
      strError,
      e.getDesc_cstr());
  }

  return false;
}

bool UndoKLCommand() 
{ 
  QString strError;

  try 
  { 
    RTVal valError = RTVal::ConstructString(
      Application::FabricApplicationStates::GetAppStates()->getContext(), 
      "");

    bool res = GetKLCommandManager().callMethod(
      "Boolean", 
      "undoCommand", 
      1, 
      &valError).getBoolean();
    
    strError = valError.getStringCString();
    if(!res || !strError.isEmpty())
    {
      Application::FabricException::Throw(
        "KLCommandHelpers::UndoKLCommand",
        strError);

      return false;
    }

    return true;
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::UndoKLCommand",
      strError,
      e.getDesc_cstr());
  }

  return false;
}

bool RedoKLCommand() 
{  
  QString strError;

  try 
  {
    RTVal valError = RTVal::ConstructString(
      Application::FabricApplicationStates::GetAppStates()->getContext(), 
      "");

    bool res = GetKLCommandManager().callMethod(
      "Boolean", 
      "redoCommand", 
      1, 
      &valError).getBoolean();

    strError = valError.getStringCString();
    if(!res || !strError.isEmpty())
    {
      Application::FabricException::Throw(
        "KLCommandHelpers::RedoKLCommand",
        strError);

      return false;      
    }

    return true;
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::RedoKLCommand",
      strError,
      e.getDesc_cstr());
  }
  
  return false;
}

QString GetKLCommandHelp(
  RTVal klCmd) 
{
  try 
  {
    return klCmd.callMethod(
      "String", 
      "getHelp", 
      0, 
      0).getStringCString();
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandHelp",
      "",
      e.getDesc_cstr());
  }

  return "";
}

QString GetKLCommandHistoryDesc(
  RTVal klCmd) 
{
  try 
  {
    return klCmd.callMethod(
      "String", 
      "getHistoryDesc", 
      0, 
      0).getStringCString();
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandHistoryDesc",
      "",
      e.getDesc_cstr());
  }

  return "";
}

void MergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal otherKLCmd)
{
  try 
  {
    klCmd.callMethod(
      "", 
      "merge", 
      1, 
      &otherKLCmd);
  }

  catch(Exception &e)
  {
    Application::FabricException::Throw(
      "KLCommandHelpers::MergeKLCommand",
      "",
      e.getDesc_cstr());
  }
}

} // namespace Commands
} // namespace FabricUI
