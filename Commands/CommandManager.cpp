//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "CommandManager.h"
#include "CommandRegistry.h"
#include "BaseScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace Application;

bool CommandManager::s_instanceFlag = false;
CommandManager* CommandManager::s_cmdManager = 0;

CommandManager::CommandManager() 
  : QObject()
{
  if(s_instanceFlag)
    FabricException::Throw(
      "CommandManager::CommandManager",
      "CommandManager singleton has already been created"
      );
   
  // Set the pointer of the CommandManager singleton
  // equal to this instance of CommandManager.
  s_cmdManager = this;
  s_instanceFlag = true;
}

CommandManager::~CommandManager() 
{
  clear();
  s_instanceFlag = false;
}

CommandManager* CommandManager::GetCommandManager()
{
  if(!s_instanceFlag)
    FabricException::Throw(
      "CommandManager::CommandManager",
      "The manager is null");

  return s_cmdManager;
}

bool CommandManager::IsInitalized()
{
  return s_instanceFlag;
}

BaseCommand* CommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  try 
  {  
    BaseCommand *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);

    if(args.size() > 0) 
      checkCommandArgs(cmd, args);

    if(doCmd) 
      doCommand(cmd);

    return cmd;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CommandManager::createCommand",
      "Cannot create command '" + cmdName + "'",
      e.what());
  }

  return 0;
}

void CommandManager::doCommand(
  BaseCommand *cmd) 
{
  if(!cmd) 
    FabricException::Throw(
      "CommandManager::doCommand",
      "BaseCommand is null");
  
  bool subCmd = m_undoStack.size() != 0 && 
    !m_undoStack[m_undoStack.size() - 1].succeeded;
  
  // If undoable command, update the stacks.
  if(cmd->canUndo() && !subCmd) 
  {
    clearRedoStack();
    pushTopCommand(cmd, false);
  }

  // Execute the command, catch any errors.
  // The command breaks if the 'doIt' method
  // returns false or throws an exception.
  try
  {
    preProcessCommandArgs(cmd);

    if(!cmd->doIt())
      cleanupUnfinishedCommandsAndThrow(
        cmd);

    postProcessCommandArgs(cmd);
  }
   
  catch(FabricException &e) 
  {
    cleanupUnfinishedCommandsAndThrow(
      cmd,
      e.what());
  }

  // If undoable command, update the stacks.
  if(cmd->canUndo())
  {
    if(subCmd)
      pushLowCommand(cmd);

    else
    {
      m_undoStack[m_undoStack.size() - 1].succeeded = true;
      
      // Inform a command has been succefully executed.
      emit commandDone(
        m_undoStack[m_undoStack.size() - 1].topLevelCmd.data());
    }
  }

  // Delete no-undoable commands.
  // else
  // {
  //   // Inform a command has been succefully executed.
  //   emit commandDone(cmd);
      
  //   delete cmd;
  //   cmd = 0;
  // }
}

void CommandManager::undoCommand() 
{
  if(m_undoStack.size() == 0)
  {
    FabricException::Throw(
      "CommandManager::undoCommand",
      "Nothing to undo",
      "",
      PRINT);

    return;
  }

  StackedCommand stackedCmd = m_undoStack[m_undoStack.size() - 1];
  BaseCommand *top = stackedCmd.topLevelCmd.data();

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0)
  { 
    // The undo breaks if the 'undoIt' method
    // returns false or throws an exception.
    for(int i = lowLevelCmdsCount; i--;)
    {
      try
      {
        if(!stackedCmd.lowLevelCmds[i]->undoIt())
          cleanupUnfinishedUndoLowCommandsAndThrow(
            i, 
            stackedCmd);

        postProcessCommandArgs(stackedCmd.lowLevelCmds[i].data());
      }
       
      catch(FabricException &e) 
      {
        cleanupUnfinishedUndoLowCommandsAndThrow(
          i, 
          stackedCmd, 
          e.what());
      }
    }
  }
  
  else 
  {
    try
    {
      if(!top->undoIt())
        FabricException::Throw(
        "CommandManager::undoCommand top", 
        "Undoing command '" + top->getName() + "'"
        );

      postProcessCommandArgs(top);
    }
     
    catch(FabricException &e) 
    {
      FabricException::Throw(
        "CommandManager::undoCommand top", 
        "Undoing command '" + top->getName() + "'",
         e.what());
    }
  }
 
  m_undoStack.pop_back();
  m_redoStack.push_back(stackedCmd);
}

void CommandManager::redoCommand() 
{
  if(m_redoStack.size() == 0)
  {
    FabricException::Throw(
      "CommandManager::redoCommand", 
      "Nothing to redo",
      "",
      PRINT);

    return;
  }

  StackedCommand stackedCmd = m_redoStack[m_redoStack.size() - 1];
  BaseCommand *top = stackedCmd.topLevelCmd.data();

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0) 
  {
    for(int i = 0; i < lowLevelCmdsCount; ++i)
    {
      try
      {
        if(!stackedCmd.lowLevelCmds[i]->redoIt())
          cleanupUnfinishedRedoLowCommandsAndThrow(
            i, 
            stackedCmd);
        
        postProcessCommandArgs(stackedCmd.lowLevelCmds[i].data());
     }
       
      catch(FabricException &e) 
      {
        cleanupUnfinishedRedoLowCommandsAndThrow(
          i, 
          stackedCmd, 
          e.what());
      }
    }
  }

  else 
  {
    try
    {
      if(!top->redoIt())
        FabricException::Throw(
        "CommandManager::redoCommand top", 
        "Undoing command '" + top->getName() + "'"
        );

      postProcessCommandArgs(top);
    }
     
    catch(FabricException &e) 
    {
      FabricException::Throw(
        "CommandManager::redoCommand top", 
        "Undoing command '" + top->getName() + "'",
         e.what());
    }
  }

  m_redoStack.pop_back();
  m_undoStack.push_back(stackedCmd);
}

void CommandManager::clear() 
{
  clearRedoStack();
  clearCommandStack(m_undoStack);
  emit cleared();
}

unsigned CommandManager::count()
{
  return unsigned(m_redoStack.size() + m_undoStack.size());
}

int CommandManager::getStackIndex()
{
  return int(m_undoStack.size() - 1);
}

BaseCommand* CommandManager::getCommandAtIndex(
  unsigned index)
{
  if(index >= 0 && index < unsigned(m_undoStack.size()))
    return m_undoStack[index].topLevelCmd.data();

  else if (index >= unsigned(m_undoStack.size()) && index < count())
    return m_redoStack[index - m_undoStack.size()].topLevelCmd.data();

  return 0;
}

QString CommandManager::getContent()
{
  QString res = QString(
    "--> BaseCommand Manager - size:"+ QString::number(count()) + 
    ", index:" + QString::number(getStackIndex()) + 
    ", undo:"+  QString::number(m_undoStack.size()) +  
    ", redo:" + QString::number(m_redoStack.size()) + 
    "\n");
  
  res += getStackContent("Undo", m_undoStack);
  res += getStackContent("Redo", m_redoStack);
 
  return res;
}

void CommandManager::checkCommandArgs(
  BaseCommand *cmd,
  const QMap<QString, QString> &args)
{ 
  BaseScriptableCommand* scriptCommand = static_cast<BaseScriptableCommand*>(cmd);
  
  if(!scriptCommand) 
    FabricException::Throw(
      "CommandManager::checkCommandArgs",
        "BaseCommand '" + cmd->getName() +  "' is created with args " + 
        "but is not implementing the BaseScriptableCommand interface"
        );

  // Try to set the arg even if not part of the specs, 
  // some commands might require this
  QMapIterator<QString, QString> ite(args);
  while(ite.hasNext()) 
  {
    ite.next();

    scriptCommand->setArg(
      ite.key(), 
      ite.value());
  }

  scriptCommand->validateSetArgs();
}

void CommandManager::clearRedoStack() 
{
  clearCommandStack(m_redoStack);
}

void CommandManager::preProcessCommandArgs(
  BaseCommand* cmd)
{
  // Do nothing.
}

void CommandManager::postProcessCommandArgs(
  BaseCommand* cmd)
{
  // Do nothing.
}

void CommandManager::clearCommandStack(
  QList<StackedCommand> &stackedCmds) 
{
  for (int i = 0; i < stackedCmds.size(); ++i)
  {
    StackedCommand stackedCmd = stackedCmds[i];

    // Check that the command hasn't been deleted before,
    // in the case we don't own it (Python wrapping)
    if(stackedCmd.topLevelCmd != 0)
    { 
      stackedCmd.topLevelCmd.clear();
      //stackedCmd.topLevelCmd = 0;

      for (int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
      {
        if(stackedCmd.lowLevelCmds[j] != 0)
        { 
          /*delete*/ stackedCmd.lowLevelCmds[j].clear();
          //stackedCmd.topLevelCmd = 0;
        }
      }
    }
  }

  stackedCmds.clear();
}

void CommandManager::pushTopCommand(
  BaseCommand *cmd,
  bool succeeded) 
{ 
  StackedCommand stackedCmd;

  QSharedPointer< BaseCommand > prt(cmd);
  stackedCmd.topLevelCmd = prt;
  stackedCmd.succeeded = succeeded;

  m_undoStack.push_back(stackedCmd);
  commandPushed(cmd, false);
}

void CommandManager::pushLowCommand(
  BaseCommand *cmd) 
{ 
  QSharedPointer< BaseCommand > prt(cmd);

  m_undoStack[m_undoStack.size() - 1].lowLevelCmds.push_back(prt);
  commandPushed(cmd, true);
}

void CommandManager::commandPushed(
  BaseCommand *cmd,
  bool isLowCmd) 
{ 
}

QString CommandManager::getStackContent(
  const QString& stackName, 
  const QList<StackedCommand>& stack)
{
  int offset = stackName == "Redo" ? m_undoStack.size() : 0;
  QString inf = stackName == "Redo" ? "-" : "+";

  QString res;
  for (int i = 0; i < stack.size(); ++i)
  {
    StackedCommand stackedCmd = stack[i];

    BaseCommand *top = stackedCmd.topLevelCmd.data();
    BaseScriptableCommand *scriptableTop = static_cast<BaseScriptableCommand *>(top);
    
    QString desc = scriptableTop 
      ? top->getName() + "\n" + scriptableTop->getArgsDescription() 
      : top->getName();

    res += QString(inf + "[" + QString::number(offset + i) + "] " + desc);
    res += "\n";

    for (int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
    {
      BaseCommand *low = stackedCmd.lowLevelCmds[j].data();
      BaseScriptableCommand *scriptableLow = static_cast<BaseScriptableCommand *>(low);
      
      QString desc = scriptableLow ? 
        low->getName() + "\n" + scriptableLow->getArgsDescription() : 
        low->getName();

      res += QString("  "+ inf + "[" + QString::number(j) + "] " + desc);
      res += "\n";
    }
  }

  return res;
}

void CommandManager::cleanupUnfinishedCommandsAndThrow(
  BaseCommand *cmd,
  const QString &error) 
{
  QString cmdForErrorLog = (cmd != 0) 
    ? cmd->getName()
    : m_undoStack[m_undoStack.size() - 1].topLevelCmd->getName();
  
  // If the failed command have been pushed, removes it.
  if(m_undoStack.size() && !m_undoStack[m_undoStack.size() - 1].succeeded) 
  {
    StackedCommand top = m_undoStack[m_undoStack.size() - 1];
    m_undoStack.removeLast();

    for(int i = top.lowLevelCmds.size(); i--;) 
    {
      if(!top.lowLevelCmds[i]->undoIt()) 
        FabricException::Throw(
          "CommandManager::cleanupUnfinishedCommandsAndThrow",
          "While reverting command '" + top.lowLevelCmds[i]->getName() + "'",
          error);
    }
  }

  FabricException::Throw(
    "CommandManager::doCommand",
    "Doing command '" + cmdForErrorLog + "'",
    error);    
}

void CommandManager::cleanupUnfinishedUndoLowCommandsAndThrow(
  int topLevelCmdIndex, 
  StackedCommand &stackedCmd,
  const QString &error) 
{
  int j = 0;

  try
  {
    for(j = topLevelCmdIndex+1; j<int(stackedCmd.lowLevelCmds.size()); ++j)
      stackedCmd.lowLevelCmds[j]->redoIt();
  }
       
  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CommandManager::cleanupUnfinishedUndoLowCommandsAndThrow",
      "Redoing command, top: '" + stackedCmd.topLevelCmd->getName() + 
      "', low: '" + stackedCmd.lowLevelCmds[j]->getName() + "'",
      error);
  }

  FabricException::Throw(
    "CommandManager::undoCommand",
    "Undoing command, top: '" + stackedCmd.topLevelCmd->getName() + 
    "', low: '" + stackedCmd.lowLevelCmds[topLevelCmdIndex]->getName() + "'",
    error);
}

void CommandManager::cleanupUnfinishedRedoLowCommandsAndThrow(
  int topLevelCmdIndex, 
  StackedCommand &stackedCmd,
  const QString &error) 
{
  int j = 0;

  try
  {
    for(j = topLevelCmdIndex; j--;)
      stackedCmd.lowLevelCmds[j]->undoIt();
  }
       
  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CommandManager::cleanupUnfinishedRedoLowCommandsAndThrow",
      "Undoing command, top: '" + stackedCmd.topLevelCmd->getName() + 
      "', low: '" + stackedCmd.lowLevelCmds[j]->getName() + "'",
      error);
  }

  FabricException::Throw(
    "CommandManager::redoCommand",
    "Redoing command, top: '" + stackedCmd.topLevelCmd->getName() + 
    "', low: '" + stackedCmd.lowLevelCmds[topLevelCmdIndex]->getName() + "'",
    error);
}
