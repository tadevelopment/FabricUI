//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseCommand.h"
#include "CommandManager.h"

using namespace FabricUI;
using namespace Commands;

BaseCommand::BaseCommand() 
  : QObject()
  , m_canMergeID(CommandManager::NoCanMergeID)
{
}

BaseCommand::~BaseCommand() 
{
}

void BaseCommand::registrationCallback(
  QString const&name,
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

bool BaseCommand::canLog() 
{
  return canUndo();
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

void BaseCommand::setCanMergeID(
  int canMergeID)
{
  m_canMergeID = canMergeID;
}

int BaseCommand::getCanMergeID()
{
  return m_canMergeID;
}

int BaseCommand::canMerge(
  BaseCommand *prevCmd) 
{
  if(prevCmd) 
  {
    if(getCanMergeID() > CommandManager::NoCanMergeID && 
      getCanMergeID() == prevCmd->getCanMergeID())
      return CommandManager::CanMerge;

    if( (getCanMergeID() + prevCmd->getCanMergeID() ) == 0)
      return CommandManager::MergeDone;
  }
  return CommandManager::NoCanMerge;
}

void BaseCommand::merge(
  BaseCommand *prevCmd) 
{
}
