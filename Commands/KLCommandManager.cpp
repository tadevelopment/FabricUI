//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "KLCommandManager.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

inline bool isKLCommand(
  Command *cmd)
{
  KLCommand *klCmd = dynamic_cast<KLCommand *>(cmd);
  KLScriptableCommand *klScriptCmd = dynamic_cast<KLScriptableCommand *>(cmd);
  return (klCmd || klScriptCmd);
}

KLCommandManager::KLCommandManager() 
  : RTValCommandManager()
  , m_klCmdUndoStackCount(0)
{
  try 
  {
    m_klCmdManager = RTVal::Create(
      FabricApplicationStates::GetAppStates()->getContext(), 
      "CommandManager", 
      0, 
      0);

    m_klCmdManager = m_klCmdManager.callMethod(
      "CommandManager", 
      "getCommandManager", 
      0, 
      0);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::KLCommandManager",
      "",
      e.getDesc_cstr());
  }
}

KLCommandManager::~KLCommandManager() 
{
}

void KLCommandManager::undoCommand() 
{
  if(m_undoStack.size() == 0)
  {
    FabricException::Throw(
      "KLCommandManager::undoCommand",
      "Nothing to redo",
      "",
      PRINT);

    return;
  }

  CommandManager::undoCommand();
  StackedCommand stackedCmd = m_redoStack[m_redoStack.size() - 1];
  m_klCmdUndoStackCount -= isKLCommand(stackedCmd.topLevelCmd.data()) ? 1 : 0;
}

void KLCommandManager::redoCommand() 
{
  if(m_redoStack.size() == 0)
  {
    FabricException::Throw(
      "KLCommandManager::redoCommand",
      "Nothing to redo",
      "",
      PRINT);

    return;
  }

  CommandManager::redoCommand();
  StackedCommand stackedCmd = m_undoStack[m_undoStack.size() - 1];
  m_klCmdUndoStackCount += isKLCommand(stackedCmd.topLevelCmd.data()) ? 1 : 0;
}

void KLCommandManager::clear() 
{
  try 
  {
    m_klCmdManager.callMethod(
      "", 
      "clear", 
      0, 0);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::clear",
      "",
      e.getDesc_cstr());
  }
  
  m_klCmdUndoStackCount = 0;
  CommandManager::clear();
}

RTVal KLCommandManager::getKLCommandManager()
{
  return m_klCmdManager;
}

QString KLCommandManager::getContent()
{
  QString res = CommandManager::getContent();

  try 
  {
    res += "\n" +
      QString(
        m_klCmdManager.callMethod(
          "String", 
          "getContent", 
          0, 
          0).getStringCString()
      );
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::getContent",
      "",
      e.getDesc_cstr());
  }

  return res;
}

void KLCommandManager::synchronizeKL() 
{
  unsigned klCmdCount;

  try
  {
    // Get the number of KL commands in the KL manager undo stack
    int klCmdCount_int = m_klCmdManager.callMethod(
      "SInt32", 
      "getStackIndex", 
      0, 
      0).getSInt32();

    // Index to size. If the KL undo stack is empty, 
    // `getStackIndex` returns -1, so it's always coherent.
    klCmdCount_int ++;
    klCmdCount = unsigned(klCmdCount_int);

    // !! Problem, KL and C++ command managers are out of synch
    if(m_klCmdUndoStackCount > klCmdCount)
      FabricException::Throw(
        "KLCommandManager::synchronizeKL",
        "KL and C++ command managers are out of synch before the synchronization");

    // Synchronize our stack with KL, two scenarios: 
    // 1. A C++/Python command is asked to be created from KL (KL AppCommand).
    // 2. A KL command is created in KL : we construct the
    //    KLCommand and KLScriptableCommand wrappers.
    unsigned klCmdUndoStackCount = m_klCmdUndoStackCount;

    for(unsigned i=klCmdUndoStackCount; i<klCmdCount; ++i)
    {
      RTVal cmdIndex = RTVal::ConstructUInt32(
        FabricApplicationStates::GetAppStates()->getContext(), 
        i);

      // Gets the KL command from the KL manager. 
      RTVal klCmd = m_klCmdManager.callMethod(
        "Command", 
        "getCommandAtIndex", 
        1, 
        &cmdIndex);

      // Check if it's an AppCommand.
      // Construct C++ commands from KL
      RTVal appCmd = RTVal::Construct(
        FabricApplicationStates::GetAppStates()->getContext(),
        "AppCommand", 
        1, 
        &klCmd);

      if(appCmd.isValid() && !appCmd.isNullObject())
        createCommandFromKLAppCommand(
          i, 
          klCmdCount, 
          appCmd);

      // KL commands have actually been 
      // created, create the C++ wrappers.
      else
        createKLCommandWrappers(
          i, 
          klCmdCount, 
          klCmd);
    } 
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::synchronizeKL",
      "",
      e.getDesc_cstr());
  }

  catch (FabricException &e) 
  {
    FabricException::Throw(
      "KLCommandManager::synchronizeKL",
      "",
      e.what());
  }

  // !! Problem, KL and C++ command managers are out of synch
  if(m_klCmdUndoStackCount > klCmdCount)
    FabricException::Throw(
      "KLCommandManager::synchronizeKL",
      "KL and C++ command managers are out of synch after the synchronization");
}

void KLCommandManager::createCommandFromKLAppCommand(
  unsigned &index,
  unsigned &klCmdCount,
  RTVal appCmd)
{
  // Gets the command name.
  QString cmdName = appCmd.callMethod(
    "String",
    "getName",
    0, 
    0).getStringCString();

  RTVal argNameList = appCmd.callMethod(
    "String[]", 
    "getArgNameList", 
    0, 
    0);

  QMap<QString, RTVal> args;
  for(unsigned i=0; i<argNameList.getArraySize(); ++i)
  {
    RTVal argNameVal = argNameList.getArrayElement(i);
    args[argNameVal.getStringCString()] = appCmd.callMethod(
      "RTVal", 
      "getArg", 
      1, 
      &argNameVal);
  }

  RTVal cmdIndex = RTVal::ConstructUInt32(
    FabricApplicationStates::GetAppStates()->getContext(), 
    index);

  // Remove the KL command
  m_klCmdManager.callMethod(
    "Boolean",
    "removeCommandAtIndex",
    1,
    &cmdIndex);

  // decrement
  index--; klCmdCount--;

  // Create and execute the C++ command.
  createCommand(cmdName, args);
}

void KLCommandManager::createKLCommandWrappers(
  unsigned &index,
  unsigned &klCmdCount,
  RTVal klCmd)
{
  RTVal scriptCmd = RTVal::Construct(
    FabricApplicationStates::GetAppStates()->getContext(),
    "BaseScriptableCommand", 
    1, 
    &klCmd);

  if(scriptCmd.isValid() && !scriptCmd.isNullObject())
  {
    KLScriptableCommand *klScriptableCmd = new KLScriptableCommand(scriptCmd);

    postProcessCommandArgs(klScriptableCmd);

    if(klScriptableCmd->canUndo() && klScriptableCmd->addToUndoStack())
    {
      pushTopCommand(klScriptableCmd, true);
      
      emit commandDone(
        m_undoStack[m_undoStack.size() - 1].topLevelCmd.data());
    }

    else if(!klScriptableCmd->canUndo() && klScriptableCmd->addToUndoStack())
    {
      RTVal cmdIndex = RTVal::ConstructUInt32(
        FabricApplicationStates::GetAppStates()->getContext(), 
        index);

      // Remove the KL command
      m_klCmdManager.callMethod(
        "Boolean",
        "removeCommandAtIndex",
        1,
        &cmdIndex);

      // decrement
      index--; klCmdCount--;
    }
  }

  else
  {
    pushTopCommand(new KLCommand(klCmd), true);

    emit commandDone(
      m_undoStack[m_undoStack.size() - 1].topLevelCmd.data());
  }
}

void KLCommandManager::clearRedoStack() 
{
  try 
  {
    m_klCmdManager.callMethod(
      "", 
      "clearRedoStack", 
      0, 
      0);
      
    CommandManager::clearRedoStack();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::clearRedoStack",
      "",
      e.getDesc_cstr());
  }
}

void KLCommandManager::pushTopCommand(
  Command *cmd,
  bool succeeded)
{
  m_klCmdUndoStackCount += isKLCommand(cmd) ? 1 : 0;

  CommandManager::pushTopCommand(
    cmd,
    succeeded);
}
