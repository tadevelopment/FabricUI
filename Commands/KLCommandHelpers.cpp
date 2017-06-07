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
  FABRIC_CATCH_BEGIN();

  RTVal klCmdManager = RTVal::Create(
    Application::FabricApplicationStates::GetAppStates()->getContext(), 
    "AppCommandManager", 
    0, 0);

  return klCmdManager.callMethod(
    "AppCommandManager", 
    "getCommandManager", 
    0, 0);

  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandManager");

  return RTVal();
}

RTVal GetKLCommandRegistry() 
{
  FABRIC_CATCH_BEGIN();

  RTVal klCmdRegistry = RTVal::Construct(
    Application::FabricApplicationStates::GetAppStates()->getContext(), 
    "AppCommandRegistry", 
    0, 0);

  return klCmdRegistry.callMethod(
    "AppCommandRegistry", 
    "getCommandRegistry", 
    0, 0);

  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandRegistry");

  return RTVal();
}

QString GetKLCommandName(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "String", 
    "getName", 
    0, 
    0).getStringCString();
  
  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandName");

  return "";
}

bool CanKLCommandUndo(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "Boolean", 
    "canUndo", 
    0, 
    0).getBoolean();

  FABRIC_CATCH_END("KLCommandHelpers::CanKLCommandUndo");

  return false;
}

bool CanKLCommandLog(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "Boolean", 
    "canLog", 
    0, 
    0).getBoolean();

  FABRIC_CATCH_END("KLCommandHelpers::CanKLCommandLog");
  
  return false;
}

bool DoKLCommand( 
  RTVal klCmd) 
{ 
  FABRIC_CATCH_BEGIN();

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
  
  QString strError = args[1].getStringCString();
  if(!strError.isEmpty())
    Application::FabricException::Throw(
      "KLCommandHelpers::DoKLCommand",
      strError);

  return true;
  
  FABRIC_CATCH_END("KLCommandHelpers::DoKLCommand");

  return false;
}

bool UndoKLCommand() 
{ 
  FABRIC_CATCH_BEGIN();
 
  RTVal valError = RTVal::ConstructString(
    Application::FabricApplicationStates::GetAppStates()->getContext(), 
    "");

  bool res = GetKLCommandManager().callMethod(
    "Boolean", 
    "undoCommand", 
    1, 
    &valError).getBoolean();
  
  QString strError = valError.getStringCString();
  if(!res || !strError.isEmpty())
    Application::FabricException::Throw(
      "KLCommandHelpers::UndoKLCommand",
      strError);

  return true;
  
  FABRIC_CATCH_END("KLCommandHelpers::UndoKLCommand");

  return false;
}

bool RedoKLCommand() 
{  
  FABRIC_CATCH_BEGIN();

  RTVal valError = RTVal::ConstructString(
    Application::FabricApplicationStates::GetAppStates()->getContext(), 
    "");

  bool res = GetKLCommandManager().callMethod(
    "Boolean", 
    "redoCommand", 
    1, 
    &valError).getBoolean();

  QString strError = valError.getStringCString();
  if(!res || !strError.isEmpty())
    Application::FabricException::Throw(
      "KLCommandHelpers::RedoKLCommand",
      strError);

  return true;

  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandHelp");

  return false;
}

QString GetKLCommandHelp(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "String", 
    "getHelp", 
    0, 
    0).getStringCString();
  
  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandHelp");

  return "";
}

QString GetKLCommandHistoryDesc(
  RTVal klCmd) 
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "String", 
    "getHistoryDesc", 
    0, 
    0).getStringCString();

  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandHistoryDesc");

  return "";
}
 
void SetKLCommandCanMergeID(
  FabricCore::RTVal klCmd,
  int canMergeID)
{
  FABRIC_CATCH_BEGIN();

  RTVal canMergeIDVal = RTVal::ConstructSInt32(
    klCmd.getContext(),
    canMergeID);

  klCmd.callMethod(
    "", 
    "setCanMergeID", 
    1, 
    &canMergeIDVal);

  FABRIC_CATCH_END("KLCommandHelpers::SetKLCommandCanMergeID");
}

int GetKLCommandCanMergeID(
  FabricCore::RTVal klCmd)
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "SInt32", 
    "getCanMergeID", 
    0, 
    0).getSInt32();
  
  FABRIC_CATCH_END("KLCommandHelpers::GetKLCommandCanMergeID");

  return -1;
}

bool CanMergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal prevKlCmd)
{
  FABRIC_CATCH_BEGIN();

  return klCmd.callMethod(
    "Boolean", 
    "canMerge", 
    1, 
    &prevKlCmd
    ).getBoolean();

  FABRIC_CATCH_END("KLCommandHelpers::CanMergeKLCommand");

  return false;
}

void MergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal prevKlCmd)
{
  FABRIC_CATCH_BEGIN();

  klCmd.callMethod(
    "", 
    "merge", 
    1, 
    &prevKlCmd);

  FABRIC_CATCH_END("KLCommandHelpers::MergeKLCommand");
}

} // namespace Commands
} // namespace FabricUI
