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
  int flag, 
  const QString &defaultValue)
{
  QString error;

  try
  {
    BaseScriptableCommand::declareArg(
      key,
      flag,
      defaultValue);
  }

  catch(CommandException &e) 
  {
    error = CommandException::Throw(
      "BaseScriptableCommand_Python::_declareArg_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

QPair<QString, bool> BaseScriptableCommand_Python::_isArg_Python(
  const QString &key,
  int flag)
{
  QPair<QString, bool> pair;
    
  try
  {
    pair.second = BaseScriptableCommand::isArg(
      key, 
      flag);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::Throw(
      "BaseScriptableCommand_Python::_isArg_Python",
      "",
      e.what(),
      NOTHING);
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
    pair.first = CommandException::Throw(
      "BaseScriptableCommand_Python::_getArg_Python",
      "",
      e.what(),
      NOTHING);
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
    error = CommandException::Throw(
      "BaseScriptableCommand_Python::_setArg_Python",
      "",
      e.what(),
      NOTHING);
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
    error = CommandException::Throw(
      "BaseScriptableCommand_Python::_validateSetArgs_Python",
      "",
      e.what(),
      NOTHING);
  }
 
  return error;
}

void BaseScriptableCommand_Python::declareArg(
  const QString &key, 
  int flag, 
  const QString &defaultValue)
{
  _declareArg_Python(
    key,
    flag,
    defaultValue);
}

bool BaseScriptableCommand_Python::isArg(
  const QString &key,
  int flag)
{
  QPair<QString, bool> pair = _isArg_Python(
    key, flag);
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
