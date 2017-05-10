//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "CommandException.h"
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

QPair<QString, BaseCommand*> KLCommandRegistry_Python::_createCommand_Python(
  const QString &cmdName)
{
  QPair<QString, BaseCommand*> pair;
  
  Command *cmd = KLCommandRegistry::createCommand(
    cmdName);

  BaseCommand* baseCmd = dynamic_cast<BaseCommand*>(
    cmd);

  if(!baseCmd)
    pair.first = CommandException::PrintOrThrow(
      "KLCommandRegistry_Python::_createCommand_Python",
      "Command '" + cmd->getName() + "' is not a BaseCommand",
      "",
      PRINT);
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

  catch (CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "KLCommandRegistry_Python::_commandIsRegistered_Python",
      "",
      e.what(),
      PRINT);
  }

  catch(Exception &e) 
  {
    error = CommandException::PrintOrThrow(
      "KLCommandRegistry_Python::_commandIsRegistered_Python",
      "",
      e.getDesc_cstr(),
      PRINT);
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

  catch (CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "KLCommandRegistry_Python::_synchronizeKL_Python",
      "",
      e.what(),
      PRINT);
  }

  catch(Exception &e) 
  {
    error = CommandException::PrintOrThrow(
      "KLCommandRegistry_Python::_synchronizeKL_Python",
      "",
      e.getDesc_cstr(),
      PRINT);
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
