//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommandHelpers.h"
#include "KLCommandManager.h"
#include "KLCommandRegistry.h"
#include <FabricUI/Util/FabricException.h>

using namespace FabricCore;

namespace FabricUI {
namespace Commands {

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
    Util::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandName",
      "",
      e.getDesc_cstr());
  }

  return "";
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
    Util::FabricException::Throw(
      "KLCommandHelpers::CanKLCommandUndo",
      "",
      e.getDesc_cstr());
  }
  
  return false;
}

bool AddKLCommandToUndoStack(
  RTVal klCmd) 
{
  try 
  {
    return klCmd.callMethod(
      "Boolean", 
      "addToUndoStack", 
      0, 
      0).getBoolean();
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
      "KLCommandHelpers::AddKLCommandToUndoStack",
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
    Util::FabricException::Throw(
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
    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());

    RTVal args[2] = { 
      klCmd, 
      RTVal::ConstructString(
        manager->getClient(), 
        "") 
    };

    manager->getKLCommandManager().callMethod(
      "", 
      "doCommand", 
      2, 
      args);
    
    strError = args[1].getStringCString();
    if(!strError.isEmpty())
      Util::FabricException::Throw(
        "KLCommandHelpers::DoKLCommand",
        strError);

    return true;
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
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
    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());

    RTVal valError = RTVal::ConstructString(
      manager->getClient(), 
      "");

    bool res = manager->getKLCommandManager().callMethod(
      "Boolean", 
      "undoCommand", 
      1, 
      &valError).getBoolean();
    
    strError = valError.getStringCString();
    if(!res || !strError.isEmpty())
    {
      Util::FabricException::Throw(
        "KLCommandHelpers::UndoKLCommand",
        strError);

      return false;
    }

    return true;
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
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
    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());
    
    RTVal valError = RTVal::ConstructString(
      manager->getClient(), 
      "");

    bool res = manager->getKLCommandManager().callMethod(
      "Boolean", 
      "redoCommand", 
      1, 
      &valError).getBoolean();

    strError = valError.getStringCString();
    if(!res || !strError.isEmpty())
    {
      Util::FabricException::Throw(
        "KLCommandHelpers::RedoKLCommand",
        strError);

      return false;      
    }

    return true;
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
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
    Util::FabricException::Throw(
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
    Util::FabricException::Throw(
      "KLCommandHelpers::GetKLCommandHistoryDesc",
      "",
      e.getDesc_cstr());
  }

  return "";
}

} // namespace Commands
} // namespace FabricUI
