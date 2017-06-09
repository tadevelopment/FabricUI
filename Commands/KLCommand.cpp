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
  return KLCommandHelpers::getKLCommandName(
    m_klCmd);
}
 
bool KLCommand::canUndo() 
{
  return KLCommandHelpers::canKLCommandUndo(
    m_klCmd);
}

bool KLCommand::canLog() 
{
  return KLCommandHelpers::canKLCommandLog(
    m_klCmd);
}

bool KLCommand::doIt() 
{ 
  FABRIC_CATCH_BEGIN();

  RTVal cmd = RTVal::Construct(
    m_klCmd.getContext(),
    "Command",
    1,
    &m_klCmd);
    
  return KLCommandHelpers::doKLCommand(cmd);
  
  FABRIC_CATCH_END("KLCommand::doIt");
 
  return false;
}

bool KLCommand::undoIt() 
{ 
  return KLCommandHelpers::undoKLCommand();
}

bool KLCommand::redoIt() 
{  
  return KLCommandHelpers::redoKLCommand();
}

QString KLCommand::getHelp() 
{
  return KLCommandHelpers::getKLCommandHelp(
    m_klCmd);
}

QString KLCommand::getHistoryDesc() 
{
  return KLCommandHelpers::getKLCommandHistoryDesc(
    m_klCmd);
}

void KLCommand::setCanMergeID(
  int canMergeID)
{
  KLCommandHelpers::setKLCommandCanMergeID(
    m_klCmd, 
    canMergeID);
}

int KLCommand::getCanMergeID()
{
  return KLCommandHelpers::getKLCommandCanMergeID(m_klCmd);
}

bool KLCommand::canMerge(
  BaseCommand *prevCmd) 
{
  KLCommand* scriptCmd = qobject_cast<KLCommand*>(prevCmd);
  if(scriptCmd == 0)
    return false;
  return KLCommandHelpers::canMergeKLCommand(m_klCmd, scriptCmd->m_klCmd);
}

void KLCommand::merge(
  BaseCommand *prevCmd) 
{
  KLCommand* scriptCmd = qobject_cast<KLCommand*>(prevCmd);
  KLCommandHelpers::mergeKLCommand(m_klCmd, scriptCmd->m_klCmd);
}
