//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "KLCommandRegistry_Python.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace Application;
 
KLCommandRegistry_Python::KLCommandRegistry_Python() 
  : KLCommandRegistry() 
{
}

KLCommandRegistry_Python::~KLCommandRegistry_Python() 
{
}

QPair<QString, BaseCommand*> KLCommandRegistry_Python::_createCommand_Python(
  const QString &cmdName)
{
  QPair<QString, BaseCommand*> pair;
  
  pair.second = KLCommandRegistry::createCommand(
     cmdName);

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

BaseCommand* KLCommandRegistry_Python::createCommand(
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
