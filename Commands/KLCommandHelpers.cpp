//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommandHelpers.h"
#include "KLCommandRegistry.h"
#include "KLCommandManager.h"

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
    printf(
      "KLCommandHelpers::GetKLCommandName: exception: %s\n", 
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
    printf(
      "KLCommandHelpers::CanKLCommandUndo: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return false;
}

bool DoKLCommand( 
  RTVal klCmd) 
{ 
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
    
    QString strError = args[1].getStringCString();

    if(!strError.isEmpty())
    {
      printf(
        "KLCommandHelpers::DoKLCommand: error: %s\n", 
        strError.toUtf8().constData());

      return false;
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "KLCommandHelpers::DoKLCommand: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
}

bool UndoKLCommand() 
{ 
  try 
  { 
    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());

    RTVal errorVal = RTVal::ConstructString(
      manager->getClient(), 
      "");

    bool res = manager->getKLCommandManager().callMethod(
      "Boolean", 
      "undoCommand", 
      1, 
      &errorVal).getBoolean();

    if(!res)
    {
      printf(
        "KLCommandHelpers::UndoKLCommand: error: %s\n", 
        errorVal.getStringCString());

      return false;
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "KLCommandHelpers::UndoKLCommand: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
}

bool RedoKLCommand() 
{  
  try 
  {
    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      Commands::CommandManager::GetCommandManager());
    
    RTVal errorVal = RTVal::ConstructString(
      manager->getClient(), 
      "");

    bool res = manager->getKLCommandManager().callMethod(
      "Boolean", 
      "redoCommand", 
      1, 
      &errorVal).getBoolean();

    if(!res)
    {
      printf(
        "KLCommandHelpers::RedoKLCommand: error: %s\n", 
        errorVal.getStringCString());

      return false;      
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "KLCommandHelpers::RedoKLCommand: exception: %s\n", 
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
    printf(
      "KLCommandHelpers::GetKLCommandHelp: exception: %s\n", 
      e.getDesc_cstr());
  }

  return "";
}

} // namespace Commands
} // namespace FabricUI
