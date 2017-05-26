//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
#include "KLCommandHelpers.h"
#include "KLCommandManager.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

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
    Application::FabricException::Throw(
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
    Application::FabricException::Throw(
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
    Application::FabricException::Throw(
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
    std::cout << "KLCommandHelpers::DoKLCommand 1" << std::endl;
    KLCommandManager *manager = qobject_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());
    std::cout << "KLCommandHelpers::DoKLCommand 2 " << bool(manager) << std::endl;

    RTVal args[2] = { 
      klCmd, 
      RTVal::ConstructString(
        Application::FabricApplicationStates::GetAppStates()->getContext(), 
        "") 
    };

    manager->getKLCommandManager().callMethod(
      "", 
      "doCommand", 
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
    std::cout << "KLCommandHelpers::UndoKLCommand 1" << std::endl;
    KLCommandManager *manager = qobject_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());
    std::cout << "KLCommandHelpers::UndoKLCommand 2 " << bool(manager) << std::endl;

    RTVal valError = RTVal::ConstructString(
      Application::FabricApplicationStates::GetAppStates()->getContext(), 
      "");

    bool res = manager->getKLCommandManager().callMethod(
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
    std::cout << "KLCommandHelpers::RedoKLCommand 1" << std::endl;
    KLCommandManager *manager = qobject_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());
    std::cout << "KLCommandHelpers::RedoKLCommand 2 " << bool(manager) << std::endl;

    RTVal valError = RTVal::ConstructString(
      Application::FabricApplicationStates::GetAppStates()->getContext(), 
      "");

    bool res = manager->getKLCommandManager().callMethod(
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

} // namespace Commands
} // namespace FabricUI
