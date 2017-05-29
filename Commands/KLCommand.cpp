//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "KLCommandHelpers.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

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
  return GetKLCommandName(m_klCmd);
}
 
bool KLCommand::canUndo() 
{
  return CanKLCommandUndo(m_klCmd);
}

bool KLCommand::addToUndoStack() 
{
  return AddKLCommandToUndoStack(m_klCmd);
}

bool KLCommand::canLog() 
{
  return CanKLCommandLog(m_klCmd);
}

bool KLCommand::doIt() 
{ 
  bool isDone = false;

  try 
  {
    RTVal cmd = RTVal::Construct(
      m_klCmd.getContext(),
      "Command",
      1,
      &m_klCmd);
    
    isDone = DoKLCommand(cmd);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLScriptableCommand::doIt", 
      "",
      e.getDesc_cstr());
  }
  
  return isDone;
}

bool KLCommand::undoIt() 
{ 
  return UndoKLCommand();
}

bool KLCommand::redoIt() 
{  
  return RedoKLCommand();
}

QString KLCommand::getHelp() 
{
  return GetKLCommandHelp(m_klCmd);
}

QString KLCommand::getHistoryDesc() 
{
  return GetKLCommandHistoryDesc(
    m_klCmd);
}
