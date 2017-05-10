//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseCommand.h"
 
using namespace FabricUI;
using namespace Commands;

BaseCommand::BaseCommand() 
{
}

BaseCommand::~BaseCommand() 
{
}

void BaseCommand::registrationCallback(
  const QString &name,
  void *userData) 
{
  m_name = name;
}

QString BaseCommand::getName() 
{
  return m_name;
}

bool BaseCommand::canUndo() 
{
  return false;
}

bool BaseCommand::doIt() 
{
  return false;
}

bool BaseCommand::undoIt() 
{
  return false;
}

bool BaseCommand::redoIt() 
{
  return false;
}

QString BaseCommand::getHelp() 
{
  return "";
}

QString BaseCommand::getHistoryDesc() 
{
  return getName();
}
