//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "CommandException.h"
#include "BaseScriptableCommand_Python.h"

using namespace FabricUI;
using namespace Commands;
 
BaseScriptableCommand_Python::BaseScriptableCommand_Python() 
  : BaseScriptableCommand() 
{
}

BaseScriptableCommand_Python::~BaseScriptableCommand_Python() 
{
}

QString BaseScriptableCommand_Python::_declareArg_Python(
  const QString &key, 
  bool optional, 
  const QString &defaultValue,
  bool loggable)
{
  QString error;

  try
  {
    BaseScriptableCommand::declareArg(
      key,
      optional,
      defaultValue,
      loggable);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseScriptableCommand_Python::_declareArg_Python",
      "",
      e.what(),
      PRINT);
  }

  return error;
}

QPair<QString, bool> BaseScriptableCommand_Python::_isArgOptional_Python(
  const QString &key)
{
  QPair<QString, bool> pair;
    
  try
  {
    pair.second = BaseScriptableCommand::isArgOptional(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseScriptableCommand_Python::_isArgOptional_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QPair<QString, bool> BaseScriptableCommand_Python::_isArgLoggable_Python(
  const QString &key)
{
  QPair<QString, bool> pair;
    
  try
  {
    pair.second = BaseScriptableCommand::isArgLoggable(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseScriptableCommand_Python::_isArgLoggable_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QPair<QString, QString> BaseScriptableCommand_Python::_getArg_Python(
  const QString &key)
{
  QPair<QString, QString> pair;
    
  try
  {
    pair.second = BaseScriptableCommand::getArg(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseScriptableCommand_Python::_getArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QString BaseScriptableCommand_Python::_setArg_Python(
  const QString &key,
  const QString &value)
{
  QString error;
    
  try
  {
    BaseScriptableCommand::setArg(
      key,
      value);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseScriptableCommand_Python::_setArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return error;
}

QString BaseScriptableCommand_Python::_validateSetArgs_Python()
{
  QString error;
    
  try
  {
    BaseScriptableCommand::validateSetArgs();
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseScriptableCommand_Python::_validateSetArgs_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return error;
}

void BaseScriptableCommand_Python::declareArg(
  const QString &key, 
  bool optional, 
  const QString &defaultValue,
  bool loggable)
{
  _declareArg_Python(
    key,
    optional,
    defaultValue,
    loggable);
}

bool BaseScriptableCommand_Python::isArgOptional(
  const QString &key)
{
  QPair<QString, bool> pair = _isArgOptional_Python(
    key);
  return pair.second;
}

bool BaseScriptableCommand_Python::isArgLoggable(
  const QString &key)
{
  QPair<QString, bool> pair = _isArgLoggable_Python(
    key);
  return pair.second;
}

QString BaseScriptableCommand_Python::getArg(
  const QString &key)
{
  QPair<QString, QString> pair = _getArg_Python(
    key);
  return pair.second;
}

void BaseScriptableCommand_Python::setArg(
  const QString &key,
  const QString &value)
{
  _setArg_Python(
    key, 
    value);
}

void BaseScriptableCommand_Python::validateSetArgs()
{
  _validateSetArgs_Python();
}
