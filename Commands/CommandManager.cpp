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

int CommandManager::NoCanMergeID = -1;
int CommandManager::NoDebug = -1;
int CommandManager::Debug = 0;
int CommandManager::VerboseDebug = 1;
bool CommandManager::s_instanceFlag = false;
CommandManager* CommandManager::s_cmdManager = 0;

CommandManager::CommandManager() 
  : QObject()
  , m_canMergeIDCounter(0)
  , m_debugMode(NoDebug)
{
  if(s_instanceFlag)
    FabricException::Throw(
      "CommandManager::CommandManager",
      "CommandManager singleton has already been created");
   
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

CommandManager* CommandManager::getCommandManager()
{
  if(!s_instanceFlag)
    FabricException::Throw(
      "CommandManager::getCommandManager",
      "The manager is null");

  return s_cmdManager;
}

bool CommandManager::isInitalized()
{
  return s_instanceFlag;
}

BaseCommand* CommandManager::createCommand(
  QString cmdName,  
  QMap<QString, QString> args, 
  bool doCmd,
  int canMergeID)
{
  FABRIC_CATCH_BEGIN();

  BaseCommand *cmd = CommandRegistry::getCommandRegistry()->createCommand(
    cmdName);

  if(args.size() > 0) 
    checkCommandArgs(cmd, args);

  if(doCmd) 
    doCommand(cmd);

  return cmd;

  FABRIC_CATCH_END("CommandManager::createCommand");

  return 0;
}

void CommandManager::doCommand(
  BaseCommand *cmd,
  int canMergeID) 
{
  if(!cmd) 
    FabricException::Throw(
      "CommandManager::doCommand",
      "BaseCommand is null");

  FABRIC_CATCH_BEGIN();
  
  cmd->setCanMergeID(canMergeID);

  bool canMerge = m_undoStack.size() > 0 && 
    cmd->canMerge(m_undoStack[m_undoStack.size()-1].topLevelCmd.data());

  bool subCmd = m_undoStack.size() != 0 && 
    !m_undoStack[m_undoStack.size()-1].succeeded;
    
  if(!subCmd && cmd->canUndo() && !canMerge)
  {
    clearRedoStack();
    pushTopCommand(cmd, false);
  }

  // Execute the command, catch any errors.
  // The command breaks if the 'doIt' method
  // returns false or throws an exception.
  try
  {
    preDoCommand(cmd);
    if(!cmd->doIt())
      cleanupUnfinishedCommandsAndThrow(cmd);
    postDoCommand(cmd);
  }
   
  catch(FabricException &e) 
  {
    cleanupUnfinishedCommandsAndThrow(cmd, e.what());
  }



  // If subCmd, push it.
  if(cmd->canUndo())
  {
    if(subCmd)
      pushLowCommand(cmd);
   
    else if(!subCmd && !canMerge)
      m_undoStack[m_undoStack.size()-1].succeeded = true;
  }

  if(canMerge)
  {
    cmd->merge(m_undoStack[m_undoStack.size()-1].topLevelCmd.data());
    QSharedPointer< BaseCommand > prt(cmd);
    m_undoStack[m_undoStack.size()-1].topLevelCmd = prt;
 
    emit commandDone(
      cmd, 
      false, // Don't add the command to the app stack, 
      true // replace the previous logged text
      );
  }

  else if(!subCmd)
  {
    emit commandDone(
      cmd, 
      cmd->canUndo(), 
      false);
  }

  if(m_debugMode != NoDebug)
    std::cout 
      << "CommandManager::doCommand, content \n"
      << getContent(m_debugMode == VerboseDebug).toUtf8().constData()
      << std::endl;
 
  FABRIC_CATCH_END("CommandManager::doCommand");
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

  StackedCommand stackedCmd = m_undoStack[m_undoStack.size()-1];
  BaseCommand *top = stackedCmd.topLevelCmd.data();

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0)
  { 
    BaseCommand *low = stackedCmd.topLevelCmd.data();

    // The undo breaks if the 'undoIt' method
    // returns false or throws an exception.
    for(int i=lowLevelCmdsCount; i--;)
    {
      try
      {
        preDoCommand(low);

        if(!low->undoIt())
          cleanupUnfinishedUndoLowCommandsAndThrow(
            i, stackedCmd);

        postDoCommand(low);
      }
       
      catch(FabricException &e) 
      {
        cleanupUnfinishedUndoLowCommandsAndThrow(
          i, stackedCmd, e.what());
      }
    }
  }
  
  else 
  {
    FABRIC_CATCH_BEGIN();

    preDoCommand( top );

    if(!top->undoIt())
      FabricException::Throw(
        "CommandManager::undoCommand", 
        "Undoing top command '" + top->getName() + "'");

    postDoCommand(top);

    FABRIC_CATCH_END("CommandManager::undoCommand top '" + top->getName()+ "'");
  }
 
  m_undoStack.pop_back();
  m_redoStack.push_back(stackedCmd);

  if(m_debugMode != NoDebug)
    std::cout 
      << "CommandManager::undoCommand, content \n"
      << getContent(m_debugMode == VerboseDebug).toUtf8().constData()
      << std::endl;
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

  StackedCommand stackedCmd = m_redoStack[m_redoStack.size()-1];
  BaseCommand *top = stackedCmd.topLevelCmd.data();

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0) 
  {
    for(int i=0; i<lowLevelCmdsCount; ++i)
    {
      BaseCommand *low = stackedCmd.topLevelCmd.data();
      try
      {
        preDoCommand(low);

        if(!low->redoIt())
          cleanupUnfinishedRedoLowCommandsAndThrow(
            i, stackedCmd);
        
        postDoCommand(low);
     }
       
      catch(FabricException &e) 
      {
        cleanupUnfinishedRedoLowCommandsAndThrow(
          i, stackedCmd, e.what());
      }
    }
  }

  else 
  {
    FABRIC_CATCH_BEGIN();

    preDoCommand( top );

    if(!top->redoIt())
      FabricException::Throw(
        "CommandManager::redoCommand", 
        "Redoing top command '" + top->getName() + "'");

    postDoCommand(top);

    FABRIC_CATCH_END("CommandManager::redoCommand top '" + top->getName()+ "'");
  }

  m_redoStack.pop_back();
  m_undoStack.push_back(stackedCmd);

  if(m_debugMode != NoDebug)
    std::cout 
      << "CommandManager::redoCommand, content \n"
      << getContent(m_debugMode == VerboseDebug).toUtf8().constData()
      << std::endl;
}

void CommandManager::clear() 
{
  clearRedoStack();
  clearCommandStack(m_undoStack);
  emit cleared();
}

int CommandManager::count()
{
  return unsigned(m_redoStack.size() + m_undoStack.size());
}

int CommandManager::totalUndoCount()
{
  int count = 0;
  for(int i=0; i<m_undoStack.size(); ++i)
  {
    StackedCommand stackedCmd = m_undoStack[i];
    count += /*top command*/1 + stackedCmd.lowLevelCmds.size();
  }
  return count;
}

int CommandManager::getStackIndex()
{
  return unsigned(m_undoStack.size()-1);
}

BaseCommand* CommandManager::getCommandAtIndex(
  int index)
{
  if(index >= 0 && index < m_undoStack.size())
    return m_undoStack[index].topLevelCmd.data();

  else if (index >= m_undoStack.size() && index < count())
    return m_redoStack[index-m_undoStack.size()].topLevelCmd.data();

  return 0;
}

int CommandManager::getNewCanMergeID()
{
  m_canMergeIDCounter++;
  return m_canMergeIDCounter;
}

void CommandManager::setDebugMode(
  int debugMode)
{
  m_debugMode = debugMode;
}

QString CommandManager::getContent(
  bool withArgs)
{
  QString res = QString(
    "--> BaseCommand Manager-size:"+ QString::number(count()) + 
    ", index:" + QString::number(getStackIndex()) + 
    ", undo:"+  QString::number(m_undoStack.size()) +  
    ", redo:" + QString::number(m_redoStack.size()) + 
    "\n");
  
  res += getStackContent("Undo", m_undoStack, withArgs);
  res += getStackContent("Redo", m_redoStack, withArgs);
 
  return res;
}

void CommandManager::checkCommandArgs(
  BaseCommand *cmd,
  QMap<QString, QString> args)
{ 
  BaseScriptableCommand* scriptCommand = qobject_cast<BaseScriptableCommand*>(cmd);

  if(!scriptCommand) 
    FabricException::Throw(
      "CommandManager::checkCommandArgs",
        "BaseCommand '" + cmd->getName() +  "' is created with args " + 
        "but is not implementing the BaseScriptableCommand interface");

  // Try to set the arg even if not part of the specs, 
  // some commands might require this
  QMapIterator<QString, QString> ite(args);
  while(ite.hasNext()) 
  {
    ite.next();
    scriptCommand->setArg(ite.key(), ite.value());
  }
  scriptCommand->validateSetArgs();
}

void CommandManager::clearRedoStack() 
{
  clearCommandStack(m_redoStack);
}

void CommandManager::preDoCommand(
  BaseCommand* cmd)
{
  // Do nothing.
}

void CommandManager::postDoCommand(
  BaseCommand* cmd)
{
  // Do nothing.
}

void CommandManager::clearCommandStack(
  QList<StackedCommand> &stackedCmds) 
{
  for(int i=0; i<stackedCmds.size(); ++i)
  {
    StackedCommand stackedCmd = stackedCmds[i];

    // Check that the command hasn't been deleted before,
    // in the case we don't own it (Python wrapping)
    if(stackedCmd.topLevelCmd != 0)
    { 
      stackedCmd.topLevelCmd.clear();
      for(int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
      {
        if(stackedCmd.lowLevelCmds[j] != 0)
          stackedCmd.lowLevelCmds[j].clear();
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
  m_undoStack[m_undoStack.size()-1].lowLevelCmds.push_back(prt);
  commandPushed(cmd, true);
}

void CommandManager::commandPushed(
  BaseCommand *cmd,
  bool isLowCmd) 
{ 
  // Do nothing.
}

QString CommandManager::getStackContent(
  QString stackName, 
  QList<StackedCommand> stack,
  bool withArgs)
{
  int offset = stackName == "Redo" ? m_undoStack.size() : 0;
  QString inf = stackName == "Redo" ? "-" : "+";

  QString res;
  for(int i=0; i<stack.size(); ++i)
  {
    StackedCommand stackedCmd = stack[i];

    BaseCommand *top = stackedCmd.topLevelCmd.data();
    BaseScriptableCommand *scriptableTop = qobject_cast<BaseScriptableCommand *>(top);

    QString desc = withArgs && scriptableTop != 0
      ? top->getName() + "\n" + scriptableTop->getArgsDescription() 
      : top->getName();

    res += QString(inf + "[" + QString::number(offset + i) + "] " + desc);
    res += "\n";

    for(int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
    {
      BaseCommand *low = stackedCmd.lowLevelCmds[j].data();
      BaseScriptableCommand *scriptableLow = qobject_cast<BaseScriptableCommand *>(low);

      QString desc = withArgs && scriptableLow != 0 
        ? low->getName() + "\n" + scriptableLow->getArgsDescription() 
        : low->getName();

      res += QString("  "+ inf + "[" + QString::number(j) + "] " + desc);
      res += "\n";
    }
  }

  return res;
}

void CommandManager::cleanupUnfinishedCommandsAndThrow(
  BaseCommand *cmd,
  QString error) 
{
  QString cmdForErrorLog = (cmd != 0) 
    ? cmd->getName()
    : m_undoStack[m_undoStack.size()-1].topLevelCmd->getName();
  
  // If the failed command have been pushed, removes it.
  if(m_undoStack.size() && !m_undoStack[m_undoStack.size()-1].succeeded) 
  {
    StackedCommand top = m_undoStack[m_undoStack.size()-1];
    m_undoStack.removeLast();

    for(int i=top.lowLevelCmds.size(); i--;) 
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
  QString error) 
{
  FABRIC_CATCH_BEGIN();

  for(int i=topLevelCmdIndex+1; i<int(stackedCmd.lowLevelCmds.size()); ++i)
    stackedCmd.lowLevelCmds[i]->redoIt();
  
  FABRIC_CATCH_END("CommandManager::cleanupUnfinishedUndoLowCommandsAndThrow");

  FabricException::Throw(
    "CommandManager::undoCommand",
    "Undoing command, top: '" + stackedCmd.topLevelCmd->getName() + 
    "', low: '" + stackedCmd.lowLevelCmds[topLevelCmdIndex]->getName() + "'",
    error);
}

void CommandManager::cleanupUnfinishedRedoLowCommandsAndThrow(
  int topLevelCmdIndex, 
  StackedCommand &stackedCmd,
  QString error) 
{
  FABRIC_CATCH_BEGIN();

  for(int i=topLevelCmdIndex; i--;)
    stackedCmd.lowLevelCmds[i]->undoIt();
 
  FABRIC_CATCH_END("CommandManager::cleanupUnfinishedRedoLowCommandsAndThrow");
 
  FabricException::Throw(
    "CommandManager::redoCommand",
    "Redoing command, top: '" + stackedCmd.topLevelCmd->getName() + 
    "', low: '" + stackedCmd.lowLevelCmds[topLevelCmdIndex]->getName() + "'",
    error);
}
