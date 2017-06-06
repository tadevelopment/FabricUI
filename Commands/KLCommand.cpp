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

bool KLCommand::canLog() 
{
  return CanKLCommandLog(m_klCmd);
}

bool KLCommand::doIt() 
{ 
  FABRIC_CATCH_BEGIN();

  RTVal cmd = RTVal::Construct(
    m_klCmd.getContext(),
    "Command",
    1,
    &m_klCmd);
    
  return DoKLCommand(cmd);
  
  FABRIC_CATCH_END("KLCommand::doIt");
 
  return false;
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
  return GetKLCommandHistoryDesc(m_klCmd);
}

void KLCommand::setCanMergeID(
  int canMergeID)
{
  SetKLCommandCanMergeID(m_klCmd, canMergeID);
}

int KLCommand::getCanMergeID()
{
  return GetKLCommandCanMergeID(m_klCmd);
}

bool KLCommand::canMerge(
  BaseCommand *cmd) 
{
  KLCommand* scriptCmd = qobject_cast<KLCommand*>(cmd);
  if(scriptCmd == 0)
    return false;
  return CanMergeKLCommand(m_klCmd, scriptCmd->m_klCmd);
}

void KLCommand::merge(
  BaseCommand *cmd) 
{
  KLCommand* scriptCmd = qobject_cast<KLCommand*>(cmd);
  MergeKLCommand(m_klCmd, scriptCmd->m_klCmd);
}