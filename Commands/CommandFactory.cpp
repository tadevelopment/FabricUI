//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandFactory.h"
 
using namespace FabricUI;
using namespace Commands;

BaseCommandFactory::BaseCommandFactory(
  void *userData) 
  : m_userData(userData)
{
}

BaseCommandFactory::~BaseCommandFactory()
{
}

BaseCommand *BaseCommandFactory::createCommand() 
{ 
  return 0; 
}
