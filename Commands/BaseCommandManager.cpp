//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include "BaseCommandManager.h"
#include "BaseScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;

bool BaseCommandManager::s_instanceFlag = false;
BaseCommandManager* BaseCommandManager::s_cmdManager = 0;

BaseCommandManager::BaseCommandManager() 
{
}

BaseCommandManager::~BaseCommandManager() 
{
  s_instanceFlag = false;
  clear();
}

void BaseCommandManager::SetCommandManagerSingleton(
  BaseCommandManager* registery)
{
  if(!s_instanceFlag)
  {
    s_cmdManager = registery;
    s_instanceFlag = true;
  }
}

BaseCommandManager* BaseCommandManager::GetCommandManager()
{
  return s_cmdManager;
}
 
BaseCommand* BaseCommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  throw(
    std::string("BaseCommandManager::createCommand, to override")
  );
  return 0;
}

void BaseCommandManager::doCommand(
  BaseCommand *cmd) 
{
  if(!cmd) 
    throw(std::string("Command is null"));
 
  bool subCmd = m_undoStack.size() != 0 && !m_undoStack[m_undoStack.size() - 1].succeeded;
  if(!subCmd) 
  {
    // push a new one
    StackedCommand stackedCmd;
    stackedCmd.topLevelCmd = cmd;
    m_undoStack.push_back(stackedCmd);
    commandPushedCallback(cmd);
  }

  if (!cmd->doIt())
  {
    //Use the name of the top-level cmd
    BaseCommand* cmdForErrorLog = (cmd != 0) ? cmd : m_undoStack[m_undoStack.size() - 1].topLevelCmd;
    throw(
      std::string(
        QString(
          "Error doing command '" + cmdForErrorLog->getName() + "'"
        ).toUtf8().constData() 
     )
   );
  } 

  else if (subCmd && cmd->canUndo())
  {
    m_undoStack[m_undoStack.size() - 1].lowLevelCmds.push_back(cmd);
    commandPushedCallback(cmd);
  }
 
  else
  {
    clearRedoStack();
    m_undoStack[m_undoStack.size() - 1].succeeded = true;
  }
}
 
void BaseCommandManager::undoCommand() 
{
  StackedCommand stackedCmd = m_undoStack[m_undoStack.size() - 1];
  BaseCommand *top = stackedCmd.topLevelCmd;

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0)
  {
    for(int i = lowLevelCmdsCount; i--;)
    {
      BaseCommand *low = stackedCmd.lowLevelCmds[i];
      if(!low->undoIt())
      {
        for(int j = i+1; j<lowLevelCmdsCount; ++j)
        {
          BaseCommand *low_ = stackedCmd.lowLevelCmds[i];
          low_->redoIt();
        }

        throw(
          std::string(
            QString(
              "Error undoing command, top: '" + top->getName() + "', low: '" + low->getName() + "'"
            ).toUtf8().constData() 
         )
       );
      }
    }
  }
  
  else if(!top->undoIt())
    throw(
      std::string(
        QString(
          "Error undoing command : " + top->getName()
        ).toUtf8().constData() 
     )
   );

  m_undoStack.pop_back();
  m_redoStack.push_back(stackedCmd);
}

void BaseCommandManager::redoCommand() 
{
  StackedCommand stackedCmd = m_redoStack[m_redoStack.size() - 1];
  BaseCommand *top = stackedCmd.topLevelCmd;

  int lowLevelCmdsCount = int(stackedCmd.lowLevelCmds.size());
  if(lowLevelCmdsCount > 0) 
  {
    for (int i = 0; i < lowLevelCmdsCount; ++i)
    {
      BaseCommand *low = stackedCmd.lowLevelCmds[i]; 
      if(!low->redoIt())
      {
        for(int j = i; j--;)
        {
          BaseCommand *low_ = stackedCmd.lowLevelCmds[i];
          low_->undoIt();
        }

        throw(
          std::string(
            QString(
              "Error redoing command, top: '" + top->getName() + "', low: '" + low->getName() + "'"
            ).toUtf8().constData()
          )
        );
      }
    }
  }

  else if(!top->redoIt())
    throw(
      std::string(
        QString(
          "Error redoing command: '" + top->getName() + "'"
        ).toUtf8().constData() 
     )
   );
 
  m_redoStack.pop_back();
  m_undoStack.push_back(stackedCmd);
}

void BaseCommandManager::clear() 
{
  clearRedoStack();
  clearCommandStack(m_undoStack);
}

int BaseCommandManager::count() const
{
  return int(m_redoStack.size() + m_undoStack.size());
}

int BaseCommandManager::getStackIndex() const
{
  return int(m_undoStack.size() - 1);
}

QString BaseCommandManager::getContent() const
{
  QString res = 
    QString(
      "--> CM size:"+ QString::number(count()) + ", index:" + QString::number(getStackIndex()) + 
      ", undo:"+  QString::number(m_undoStack.size()) +  ", redo:" + QString::number(m_redoStack.size()) + "\n"
    );
  res += getStackContent("Undo", m_undoStack);
  res += getStackContent("Redo", m_redoStack);
  return res;
}

void BaseCommandManager::checkCommandArgs(
  BaseCommand *cmd,
  const QMap<QString, QString> &args) const
{
  if(args.size() > 0) 
  {
    BaseScriptableCommand* scriptCommand = dynamic_cast<BaseScriptableCommand*>(cmd);
    if(!scriptCommand) 
      throw(
        std::string(
          QString(
            "Command '" + cmd->getName() + "' is created with args, but is not implementing the ScriptableCommand interface"
          ).toUtf8().constData() 
        )
      );

    // Try to set the arg even if not part of the specs, some commands might require this
    QMapIterator<QString, QString> ite(args);
    while (ite.hasNext()) 
    {
      ite.next();
      scriptCommand->setArg(ite.key(), ite.value());
    }

    scriptCommand->validateSetArgs();
  }
}

void BaseCommandManager::commandPushedCallback(
  BaseCommand *cmd) 
{
}

void BaseCommandManager::clearRedoStack() 
{
  clearCommandStack(m_redoStack);
}

void BaseCommandManager::clearCommandStack(
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

QString BaseCommandManager::getStackContent(
  const QString& stackName, 
  const QList<StackedCommand>& stack) const
{
  QString res;

  int offset = stackName == "Redo" ? m_undoStack.size() : 0;

  for (int i = 0; i < stack.size(); ++i)
  {
    StackedCommand stackedCmd = stack[i];

    BaseCommand *top = stackedCmd.topLevelCmd;
    BaseScriptableCommand *scriptableTop = dynamic_cast<BaseScriptableCommand *>(top);
    QString desc = scriptableTop ? top->getName() + "\n" + scriptableTop->getArgsDescription() : top->getName();
    res += QString(stackName + " Top[" + QString::number(offset + i) + "] : " + desc + "\n");

    for (int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
    {
      BaseCommand *low = stackedCmd.lowLevelCmds[j];
      BaseScriptableCommand *scriptableLow = dynamic_cast<BaseScriptableCommand *>(low);
      QString desc = scriptableLow ? low->getName() + "\n" + scriptableLow->getArgsDescription() : low->getName();
      res += QString("    Low[" + QString::number(j) + "] : " + desc + "\n");
    }
  }

  return res;
}
