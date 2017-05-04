//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "CommandHelpers.h"
#include "CommandManager.h"
#include "CommandRegistry.h"
#include "ScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;

bool CommandManager::s_instanceFlag = false;
CommandManager* CommandManager::s_cmdManager = 0;

CommandManager::CommandManager() 
  : QObject()
{
  if(s_instanceFlag)
    printAndThrow("CommandManager::CommandManager, CommandManager singleton has already been created");
   
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
    printAndThrow(
      "CommandManager::CommandManager, the manager is null"
    );

  return s_cmdManager;
}

Command* CommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  try 
  {  
    Command *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);
    
    if(args.size() > 0) 
      checkCommandArgs(cmd, args);

    if(doCmd) 
      doCommand(cmd);

    return cmd;
  }

  catch(std::string &e) 
  {
    printAndThrow(
      QString(
        "CommandManager::createCommand, error: Cannot create command '" + 
        cmdName + "', " + 
        QString(e.c_str())
      ).toUtf8().constData()
    );
  }

  return 0;
}

void CommandManager::doCommand(
  Command *cmd) 
{
  if(!cmd) 
    printAndThrow(
      "CommandManager::doCommand, Command is null"
    );
  
  bool subCmd = m_undoStack.size() != 0 && 
                !m_undoStack[m_undoStack.size() - 1].succeeded;
  
  if(cmd->canUndo() && !subCmd) 
  {
    clearRedoStack();
    pushTopCommand(cmd, false);
  }

  if(!cmd->doIt())
  {
    QString cmdForErrorLog = (cmd != 0) 
      ? cmd->getName()
      : m_undoStack[m_undoStack.size() - 1].topLevelCmd->getName();
    
    cleanupUnfinishedCommands();

    printAndThrow(
      QString(
        "CommandManager::doCommand, error doing command '" + 
        cmdForErrorLog + "'"
      ).toUtf8().constData() 
    );
  } 
  
  if(cmd->canUndo())
  {
    if(subCmd)
      pushLowCommand(cmd);
    else
    {
      m_undoStack[m_undoStack.size() - 1].succeeded = true;
      emit commandPushed(
        m_undoStack[m_undoStack.size() - 1].topLevelCmd);
    }
  }
}

void CommandManager::undoCommand() 
{
  if(m_undoStack.size() == 0)
  {
    printf("CommandManager::undoCommand: nothing to undo");
    return;
  }

  StackedCommand stackedCmd = m_undoStack[m_undoStack.size() - 1];
  Command *top = stackedCmd.topLevelCmd;

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0)
  {
    for(int i = lowLevelCmdsCount; i--;)
    {
      Command *low = stackedCmd.lowLevelCmds[i];
      if(!low->undoIt())
      {
        for(int j = i+1; j<lowLevelCmdsCount; ++j)
        {
          Command *low_ = stackedCmd.lowLevelCmds[j];
          low_->redoIt();
        }

        printAndThrow(
          QString(
            "CommandManager::undoCommand, error undoing command, top: '" + 
            top->getName() + "', low: '" + low->getName() + "'"
          ).toUtf8().constData() 
        );
      }
    }
  }
  
  else if(!top->undoIt())
    printAndThrow(
      QString(
        "CommandManager::undoCommand, error undoing command : " + 
        top->getName()
      ).toUtf8().constData() 
   );

  m_undoStack.pop_back();
  m_redoStack.push_back(stackedCmd);
}

void CommandManager::redoCommand() 
{
  if(m_redoStack.size() == 0)
  {
    printf("CommandManager::redoCommand: nothing to redo");
    return;
  }

  StackedCommand stackedCmd = m_redoStack[m_redoStack.size() - 1];
  Command *top = stackedCmd.topLevelCmd;

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0) 
  {
    for (int i = 0; i < lowLevelCmdsCount; ++i)
    {
      Command *low = stackedCmd.lowLevelCmds[i]; 
      if(!low->redoIt())
      {
        for(int j = i; j--;)
        {
          Command *low_ = stackedCmd.lowLevelCmds[j];
          low_->undoIt();
        }

        printAndThrow(
          QString(
            "CommandManager::redoCommand, error redoing command, top: '" + 
            top->getName() + "', low: '" + low->getName() + "'"
          ).toUtf8().constData()
        );
      }
    }
  }

  else if(!top->redoIt())
    printAndThrow(
      QString(
        "CommandManager::redoCommand, error redoing command: '" + 
        top->getName() + "'"
      ).toUtf8().constData() 
   );
 
  m_redoStack.pop_back();
  m_undoStack.push_back(stackedCmd);
}

void CommandManager::clear() 
{
  clearRedoStack();
  clearCommandStack(m_undoStack);
  emit cleared();
}

int CommandManager::count()
{
  return int(m_redoStack.size() + m_undoStack.size());
}

int CommandManager::getStackIndex()
{
  return int(m_undoStack.size() - 1);
}

QString CommandManager::getContent()
{
  QString res = QString(
    "--> Command Manager - size:"+ QString::number(count()) + 
    ", index:" + QString::number(getStackIndex()) + 
    ", undo:"+  QString::number(m_undoStack.size()) +  
    ", redo:" + QString::number(m_redoStack.size()) + 
    "\n");
  
  res += getStackContent("Undo", m_undoStack);
  res += getStackContent("Redo", m_redoStack);
 
  return res;
}

void CommandManager::checkCommandArgs(
  Command *cmd,
  const QMap<QString, QString> &args)
{ 
  ScriptableCommand* scriptCommand = dynamic_cast<ScriptableCommand*>(cmd);
  
  if(!scriptCommand) 
    printAndThrow(
      QString(
        "CommandManager::checkCommandArgs, command '" + cmd->getName() + 
        "' is created with args, but is not implementing the ScriptableCommand interface"
      ).toUtf8().constData() 
    );

  // Try to set the arg even if not part of the specs, 
  // some commands might require this
  QMapIterator<QString, QString> ite(args);
  while (ite.hasNext()) 
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
      delete stackedCmd.topLevelCmd;
      stackedCmd.topLevelCmd = 0;

      for (int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
      {
        if(stackedCmd.lowLevelCmds[j] != 0)
        { 
          delete stackedCmd.lowLevelCmds[j];
          stackedCmd.topLevelCmd = 0;
        }
      }
    }
  }

  stackedCmds.clear();
}

void CommandManager::pushTopCommand(
  Command *cmd,
  bool succeeded) 
{ 
  StackedCommand stackedCmd;
  stackedCmd.topLevelCmd = cmd;
  stackedCmd.succeeded = succeeded;
  m_undoStack.push_back(stackedCmd);
  commandIsPushed(cmd, false);
}

void CommandManager::pushLowCommand(
  Command *cmd) 
{ 
  m_undoStack[m_undoStack.size() - 1].lowLevelCmds.push_back(cmd);
  commandIsPushed(cmd, true);
}

void CommandManager::commandIsPushed(
  Command *cmd,
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

    Command *top = stackedCmd.topLevelCmd;
    ScriptableCommand *scriptableTop = dynamic_cast<ScriptableCommand *>(top);
    
    QString desc = scriptableTop 
      ? top->getName() + "\n" + scriptableTop->getArgsDescription() 
      : top->getName();

    res += QString(inf + "[" + QString::number(offset + i) + "] " + desc);
    res += "\n";

    for (int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
    {
      Command *low = stackedCmd.lowLevelCmds[j];
      ScriptableCommand *scriptableLow = dynamic_cast<ScriptableCommand *>(low);
      
      QString desc = scriptableLow ? 
        low->getName() + "\n" + scriptableLow->getArgsDescription() : 
        low->getName();

      res += QString("  "+ inf + "[" + QString::number(j) + "] " + desc);
      res += "\n";
    }
  }

  return res;
}

void CommandManager::cleanupUnfinishedCommands() 
{
  if(m_undoStack.size() && !m_undoStack[m_undoStack.size() - 1].succeeded) 
  {
    StackedCommand top = m_undoStack[m_undoStack.size() - 1];
    m_undoStack.removeLast();

    for(int i = top.lowLevelCmds.size(); i--;) 
    {
      if(!top.lowLevelCmds[i]->undoIt()) 
        printAndThrow(
          QString(
            "CommandManager::cleanupUnfinishedCommands, error while reverting command '" + 
             top.lowLevelCmds[i]->getName() 
          ).toUtf8().constData()
        );
    }
  }
}
