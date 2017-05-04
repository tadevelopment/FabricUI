//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "KLCommandHelpers.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

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

bool KLCommand::doIt() 
{ 
  return DoKLCommand(m_klCmd);
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
