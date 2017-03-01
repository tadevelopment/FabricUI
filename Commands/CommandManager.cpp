//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include "KLCommand.h"
#include "CommandManager.h"
#include "KLScriptableCommand.h"
#include "BaseScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

bool CommandManager::s_instanceFlag = false;
CommandManager* CommandManager::s_cmdManager = 0;

CommandManager::CommandManager(
  Client client) 
  : QObject()
  , m_client(client)
{
  if(s_instanceFlag)
    throw("CommandManager::CommandManager, CommandManager singleton has already been created");
  
  try 
  {
    m_klCmdManager = RTVal::Create(
      m_client, 
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
    printf(
      "CommandManager::CommandManager: exception: %s\n", 
      e.getDesc_cstr());
  }

  /// Registers our-self, needed in python.
  QObject::connect(
    CommandRegistry::GetCommandRegistry(),
    SIGNAL(
      commandRegisteredCallback(
        const QString &,
        const QString &,
        const QString &)
      ),
    this,
    SLOT(
      onCommandRegistered(
        const QString &,
        const QString &,
        const QString &)
      )
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
    throw(
      std::string("CommandManager::CommandManager, the manager is null")
    );

  return s_cmdManager;
}

BaseCommand* CommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  try 
  {  
    BaseCommand *cmd = CommandRegistry::GetCommandRegistry()->createCommand(cmdName);
    
    if(args.size() > 0) 
      checkCommandArgs(cmd, args);

    if (doCmd) 
      this->doCommand(cmd);
  
    return cmd;
  }

  catch(std::string &e) {
    throw(
      std::string(
        QString("Cannot create command '" + cmdName + "', " + QString(e.c_str())).toUtf8().constData()
      )
    );
  }

  return 0;
}

void CommandManager::doCommand(
  BaseCommand *cmd) 
{
  if(!cmd) 
    throw(std::string("CommandManager::doCommand, Command is null"));
 
  bool subCmd = m_undoStack.size() != 0 && !m_undoStack[m_undoStack.size() - 1].succeeded;
  if(!subCmd) 
  {
    // Clear all the redo-able commands
    clearRedoStack();
    // push a new one
    pushTopCommand(cmd, false);
  }

  if (!cmd->doIt())
  {
    //Use the name of the top-level cmd
    BaseCommand* cmdForErrorLog = (cmd != 0) ? cmd : m_undoStack[m_undoStack.size() - 1].topLevelCmd;
    throw(
      std::string(
        QString(
          "CommandManager::doCommand, error doing command '" + cmdForErrorLog->getName() + "'"
        ).toUtf8().constData() 
     )
   );
  } 

  else if (subCmd && cmd->canUndo())
    pushLowCommand(cmd);
 
  else
    m_undoStack[m_undoStack.size() - 1].succeeded = true;
}

void CommandManager::undoCommand() 
{
  if(m_undoStack.size() == 0)
  {
    printf("CommandManager::undoCommand: nothing to undo");
    return;
  }

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
          BaseCommand *low_ = stackedCmd.lowLevelCmds[j];
          low_->redoIt();
        }

        throw(
          std::string(
            QString(
              "CommandManager::undoCommand, error undoing command, top: '" + 
              top->getName() + "', low: '" + low->getName() + "'"
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
          "CommandManager::undoCommand, error undoing command : " + 
          top->getName()
        ).toUtf8().constData() 
     )
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
          BaseCommand *low_ = stackedCmd.lowLevelCmds[j];
          low_->undoIt();
        }

        throw(
          std::string(
            QString(
              "CommandManager::redoCommand, error redoing command, top: '" + 
              top->getName() + "', low: '" + low->getName() + "'"
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
          "CommandManager::redoCommand, error redoing command: '" + 
          top->getName() + "'"
        ).toUtf8().constData() 
     )
   );
 
  m_redoStack.pop_back();
  m_undoStack.push_back(stackedCmd);
}

void CommandManager::clear() 
{
  // Clear the KL command manager
  try 
  {
    m_klCmdManager.callMethod(
      "", 
      "clear", 
      0, 
      0);
  }

  catch(Exception &e)
  {
    printf(
      "CommandManager::clear: exception: %s\n", 
      e.getDesc_cstr());
  }

  // Clear our-self
  clearRedoStack();
  clearCommandStack(m_undoStack);
}

int CommandManager::count()
{
  return int(m_redoStack.size() + m_undoStack.size());
}

int CommandManager::getStackIndex()
{
  return int(m_undoStack.size() - 1);
}

RTVal CommandManager::getKLCommandManager()
{
  return m_klCmdManager;
}

Client CommandManager::getFabricClient()
{
  return m_client;
}

void CommandManager::synchronizeKL() 
{
  // Get the number of KL commands 
  // in the KL manager undo stack
  int klCmdCount = 0;
  try
  {
    klCmdCount = m_klCmdManager.callMethod(
      "SInt32", 
      "getStackIndex", 
      0, 
      0).getSInt32();

    klCmdCount ++;
  }

  catch(Exception &e)
  {
    printf(
      "CommandManager::synchronizeKL: exception: %s\n", 
      e.getDesc_cstr());
  }

  // Get the number of undo KL commands 
  // in this manager
  int cppKLCmdCount = 0;
  for(int i=0; i<m_undoStack.size(); ++i)
  {
    KLCommand *cmd = dynamic_cast<KLCommand *>(m_undoStack[i].topLevelCmd);
    KLScriptableCommand *scriptCmd = dynamic_cast<KLScriptableCommand *>(m_undoStack[i].topLevelCmd);
    cppKLCmdCount += (cmd || scriptCmd) ? 1 : 0;
  } 

  // Synchronize our stack with KL
  if (cppKLCmdCount < klCmdCount)
  {
    try
    {
      for(int i=cppKLCmdCount; i<klCmdCount; ++i)
      {
        BaseCommand *cmd = 0;

        RTVal cmdIndex = RTVal::ConstructUInt32(m_client, i);

        // Gets the KL command from the KL manager. 
        // Check if it's a scriptable command
        RTVal klCmd = m_klCmdManager.callMethod(
          "BaseScriptableCommand", 
          "getCommandAtIndex", 
          1, 
          &cmdIndex);

        if(!klCmd.isNullObject())
          cmd = new KLScriptableCommand(klCmd);
        
        // if not, it's a simple command.
        else
        {
          klCmd = m_klCmdManager.callMethod(
            "Command", 
            "getCommandAtIndex", 
            1, 
            &cmdIndex);

          cmd = new KLCommand(klCmd);
        }

        // Push the command
        pushTopCommand(cmd, true);
      } 
    }

    catch(Exception &e)
    {
      printf(
        "CommandManager::synchronizeKL: exception: %s\n", 
        e.getDesc_cstr());
    }
  }

  // !! Problem, KL and C++ command managers are out of synch
  else if(cppKLCmdCount > klCmdCount)
    throw(
      std::string(
        QString(
          "CommandManager::synchronizeKL: the KL and C++ command managers are out of synch"
        ).toUtf8().constData() 
      )
    );
}

QString CommandManager::getContent()
{
  QString res = 
    QString(
      "--> Command Manager - size:"+ QString::number(count()) + ", index:" + QString::number(getStackIndex()) + 
      ", undo:"+  QString::number(m_undoStack.size()) +  ", redo:" + QString::number(m_redoStack.size()) + "\n"
    );
  res += getStackContent("Undo", m_undoStack);
  res += getStackContent("Redo", m_redoStack);

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
    printf(
      "CommandManager::getContent: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

void CommandManager::checkCommandArgs(
  BaseCommand *cmd,
  const QMap<QString, QString> &args)
{
  if(args.size() > 0) 
  {
    BaseScriptableCommand* scriptCommand = dynamic_cast<BaseScriptableCommand*>(cmd);
    
    if(!scriptCommand) 
      throw(
        std::string(
          QString(
            "CommandManager::checkCommandArgs, command '" + cmd->getName() + "' is created with args, but is not implementing the ScriptableCommand interface"
          ).toUtf8().constData() 
        )
      );

    // Try to set the arg even if not part of the specs, 
    // some commands might require this
    QMapIterator<QString, QString> ite(args);
    while (ite.hasNext()) 
    {
      ite.next();
      scriptCommand->setArg(ite.key(), ite.value());
    }

    scriptCommand->validateSetArgs();
  }
}

void CommandManager::onCommandRegistered(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType)
{
}

void CommandManager::clearRedoStack() 
{
  try 
  {
    m_klCmdManager.callMethod(
      "", 
      "clearRedoStack", 
      0, 
      0);
  }

  catch(Exception &e)
  {
    printf(
      "CommandManager::clearRedoStack: exception: %s\n", 
      e.getDesc_cstr());
  }

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
  BaseCommand *cmd,
  bool succeeded) 
{ 
  StackedCommand stackedCmd;
  stackedCmd.topLevelCmd = cmd;
  stackedCmd.succeeded = succeeded;
  m_undoStack.push_back(stackedCmd);
  commandPushed(cmd, false);
}

void CommandManager::pushLowCommand(
  BaseCommand *cmd) 
{ 
  m_undoStack[m_undoStack.size() - 1].lowLevelCmds.push_back(cmd);
  commandPushed(cmd, true);
}

void CommandManager::commandPushed(
  BaseCommand *cmd,
  bool isLowCmd) 
{ 
  emit commandPushedCallback(
    cmd,
    isLowCmd);
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

    BaseCommand *top = stackedCmd.topLevelCmd;
    BaseScriptableCommand *scriptableTop = dynamic_cast<BaseScriptableCommand *>(top);
    
    QString desc = scriptableTop 
      ? top->getName() + "\n" + scriptableTop->getArgsDescription() 
      : top->getName();

    res += QString(inf + "[" + QString::number(offset + i) + "] " + desc);
    res += "\n";

    for (int j = 0; j < stackedCmd.lowLevelCmds.size(); ++j)
    {
      BaseCommand *low = stackedCmd.lowLevelCmds[j];
      BaseScriptableCommand *scriptableLow = dynamic_cast<BaseScriptableCommand *>(low);
      
      QString desc = scriptableLow ? 
        low->getName() + "\n" + scriptableLow->getArgsDescription() : 
        low->getName();

      res += QString("  "+ inf + "[" + QString::number(j) + "] " + desc);
      res += "\n";
    }
  }

  return res;
}
