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
  // Delete the user data if needed.
  if(m_userData != 0)
  {
    delete m_userData;
    m_userData = 0;
  }
}

BaseCommand *BaseCommandFactory::createCommand() 
{ 
  throw(
    std::string("BaseCommandFactory::createCommand, to override")
  );
  return 0; 
}
