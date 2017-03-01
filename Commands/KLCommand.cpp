//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "CommandManager.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

QString KLCommand::GetName(
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
      "KLCommand::GetName: exception: %s\n", 
      e.getDesc_cstr());
  }

  return "";
}

bool KLCommand::CanUndo(
  RTVal klCmd) 
{
  try 
  {
    return klCmd.callMethod(
      "String", 
      "canUndo", 
      0, 
      0).getBoolean();
  }
  catch(Exception &e)
  {
    printf(
      "KLCommand::CanUndo: exception: %s\n", 
      e.getDesc_cstr());
  }
  return false;
}

bool KLCommand::DoIt( 
  RTVal klCmd) 
{ 
  try 
  {
    RTVal args[2] = { 
      klCmd, 
      RTVal::ConstructString(
        CommandManager::GetCommandManager()->getFabricClient(), 
        "") 
    };

    CommandManager::GetCommandManager()->getKLCommandManager().callMethod(
      "", 
      "doCommand", 
      2, 
      args);
    
    QString strError = args[1].getStringCString();

    if(!strError.isEmpty())
    {
      printf(
        "KLCommand::DoIt: error: %s\n", 
        strError.toUtf8().constData());

      return false;
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "KLCommand::DoIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
}

bool KLCommand::UndoIt() 
{ 
  try 
  {    
    RTVal errorVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      "");

    bool res = CommandManager::GetCommandManager()->getKLCommandManager().callMethod(
      "Boolean", 
      "undoCommand", 
      1, 
      &errorVal).getBoolean();

    if(!res)
    {
      printf(
        "KLCommand::UndoIt: error: %s\n", 
        errorVal.getStringCString());

      return false;
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "KLCommand::UndoIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
}

bool KLCommand::RedoIt() 
{  
  try 
  {
    RTVal errorVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      "");

    bool res = CommandManager::GetCommandManager()->getKLCommandManager().callMethod(
      "Boolean", 
      "redoCommand", 
      1, 
      &errorVal).getBoolean();

    if(!res)
    {
      printf(
        "KLCommand::RedoIt: error: %s\n", 
        errorVal.getStringCString());

      return false;      
    }

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "KLCommand::RedoIt: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return false;
}

KLCommand::KLCommand(
  FabricCore::RTVal klCmd)
  : BaseCommand() 
  , m_klCmd(klCmd)
{
}

KLCommand::~KLCommand() 
{
}

QString KLCommand::getName() 
{
  return KLCommand::GetName(m_klCmd);
}

bool KLCommand::canUndo() 
{
  return KLCommand::CanUndo(m_klCmd);
}

bool KLCommand::doIt() 
{ 
  return KLCommand::DoIt(m_klCmd);
}

bool KLCommand::undoIt() 
{ 
  return KLCommand::UndoIt();
}

bool KLCommand::redoIt() 
{  
  return KLCommand::RedoIt();
}
