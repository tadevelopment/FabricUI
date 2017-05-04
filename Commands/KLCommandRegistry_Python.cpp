//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "CommandHelpers.h"
#include "KLCommandRegistry_Python.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

KLCommandRegistry_Python::KLCommandRegistry_Python(
  Client client) 
  : KLCommandRegistry(client) 
{
}

KLCommandRegistry_Python::~KLCommandRegistry_Python() 
{
}

BaseCommand* KLCommandRegistry_Python::_createCommand_Python(
  const QString &cmdName)
{
  Command *cmd = KLCommandRegistry::createCommand(
    cmdName);

  BaseCommand* baseCmd = dynamic_cast<BaseCommand*>(
    cmd);

  if(!baseCmd)
    printAndThrow(
      "KLCommandRegistry_Python::_createCommand_Python, cmd is not a BaseCommand");
  
  return baseCmd;
}

Command* KLCommandRegistry_Python::createCommand(
  const QString &cmdName)
{
  return _createCommand_Python(
    cmdName);
}
