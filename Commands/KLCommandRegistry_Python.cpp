//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "KLCommandRegistry_Python.h"
#include <FabricUI/Util/FabricException.h>

using namespace FabricUI;
using namespace Util;
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

QPair<QString, BaseCommand*> KLCommandRegistry_Python::_createCommand_Python(
  const QString &cmdName)
{
  QPair<QString, BaseCommand*> pair;
  
  Command *cmd = KLCommandRegistry::createCommand(
    cmdName);

  BaseCommand* baseCmd = dynamic_cast<BaseCommand*>(
    cmd);

  if(!baseCmd)
    pair.first = FabricException::Throw(
      "KLCommandRegistry_Python::_createCommand_Python",
      "Command '" + cmd->getName() + "' is not a BaseCommand",
      "",
      PRINT | THROW);
  else
    pair.second = baseCmd;

  return pair;
}

QString KLCommandRegistry_Python::_commandIsRegistered_Python(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType)
{
  QString error;

  try
  {
    KLCommandRegistry::commandIsRegistered(
      cmdName,
      cmdType,
      implType);
  }

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
      "KLCommandRegistry_Python::_commandIsRegistered_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

QString KLCommandRegistry_Python::_synchronizeKL_Python()
{
  QString error;

  try
  {
    KLCommandRegistry::synchronizeKL();
  }

  catch (FabricException &e) 
  {
    error = FabricException::Throw(
      "KLCommandRegistry_Python::_synchronizeKL_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

Command* KLCommandRegistry_Python::createCommand(
  const QString &cmdName)
{ 
  QPair<QString, BaseCommand*> pair = _createCommand_Python(
    cmdName);

  return pair.second;
}

void KLCommandRegistry_Python::commandIsRegistered(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType)
{
  _commandIsRegistered_Python(
    cmdName,
    cmdType,
    implType);
}

void KLCommandRegistry_Python::synchronizeKL()
{
  _synchronizeKL_Python();
}
