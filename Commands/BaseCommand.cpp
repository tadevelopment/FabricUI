//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseCommand.h"
#include <typeinfo>

using namespace FabricUI;
using namespace Commands;

BaseCommand::BaseCommand() 
{
}

BaseCommand::~BaseCommand() 
{
}

void BaseCommand::registrationCallBack(
	const QString &name,
	void *userData) 
{
  m_name = name;
}

const QString & BaseCommand::getName() const 
{
  return m_name;
}

bool BaseCommand::canUndo() const 
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
