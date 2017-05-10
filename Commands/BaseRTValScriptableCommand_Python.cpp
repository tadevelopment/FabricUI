//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "CommandException.h"
#include "BaseRTValScriptableCommand_Python.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

BaseRTValScriptableCommand_Python::BaseRTValScriptableCommand_Python() 
  : BaseRTValScriptableCommand() 
{
}

BaseRTValScriptableCommand_Python::~BaseRTValScriptableCommand_Python() 
{
}

QString BaseRTValScriptableCommand_Python::_declareArg_Python(
  const QString &key, 
  bool optional, 
  const QString &defaultValue,
  bool loggable)
{
  QString error;

  try
  {
    BaseRTValScriptableCommand::declareArg(
      key,
      optional,
      defaultValue,
      loggable);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_declareArg_Python",
      "",
      e.what(),
      PRINT);
  }

  return error;
}

QPair<QString, bool> BaseRTValScriptableCommand_Python::_isArgOptional_Python(
  const QString &key)
{
  QPair<QString, bool> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::isArgOptional(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_isArgOptional_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QPair<QString, bool> BaseRTValScriptableCommand_Python::_isArgLoggable_Python(
  const QString &key)
{
  QPair<QString, bool> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::isArgLoggable(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_isArgLoggable_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QPair<QString, QString> BaseRTValScriptableCommand_Python::_getArg_Python(
  const QString &key)
{
  QPair<QString, QString> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::getArg(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_getArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QString BaseRTValScriptableCommand_Python::_setArg_Python(
  const QString &key,
  const QString &value)
{
  QString error;
    
  try
  {
    BaseRTValScriptableCommand::setArg(
      key,
      value);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_setArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return error;
}

QString BaseRTValScriptableCommand_Python::_validateSetArgs_Python()
{
  QString error;
    
  try
  {
    BaseRTValScriptableCommand::validateSetArgs();
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_validateSetArgs_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return error;
}
  
QString BaseRTValScriptableCommand_Python::_declareRTValArg_Python(
  const QString &key, 
  const QString &type,
  bool optional, 
  RTVal defaultValue,
  bool loggable)
{
  QString error;

  try
  {
    BaseRTValScriptableCommand::declareRTValArg(
      key,
      type,
      optional,
      defaultValue,
      loggable);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_declareRTValArg_Python",
      "",
      e.what(),
      PRINT);
  }

  return error;
}

QPair<QString, QString> BaseRTValScriptableCommand_Python::_getRTValArgType_Python(
  const QString &key)
{
  QPair<QString, QString> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::getRTValArgType(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_getRTValArgType_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QString BaseRTValScriptableCommand_Python::_setRTValArgType_Python(
  const QString &key,
  const QString &type)
{
  QString error;
    
  try
  {
    BaseRTValScriptableCommand::setRTValArgType(
      key,
      type);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_setRTValArgType_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return error;
}

QPair<QString, RTVal> BaseRTValScriptableCommand_Python::_getRTValArg_Python(
  const QString &key)
{
  QPair<QString, RTVal> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::getRTValArg(key);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_getRTValArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QPair<QString, RTVal> BaseRTValScriptableCommand_Python::_getRTValArg_Python(
  const QString &key,
  const QString &type)
{
  QPair<QString, RTVal> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::getRTValArg(
      key,
      type);
  }

  catch(CommandException &e) 
  {
    pair.first = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_getRTValArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return pair;
}

QString BaseRTValScriptableCommand_Python::_setRTValArg_Python(
  const QString &key,
  RTVal value)
{
  QString error;
    
  try
  {
    BaseRTValScriptableCommand::setRTValArg(
      key,
      value);
  }

  catch(CommandException &e) 
  {
    error = CommandException::PrintOrThrow(
      "BaseRTValScriptableCommand_Python::_setRTValArg_Python",
      "",
      e.what(),
      PRINT);
  }
 
  return error;
} 

void BaseRTValScriptableCommand_Python::declareArg(
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

bool BaseRTValScriptableCommand_Python::isArgOptional(
  const QString &key)
{
  QPair<QString, bool> pair = _isArgOptional_Python(
    key);
  return pair.second;
}

bool BaseRTValScriptableCommand_Python::isArgLoggable(
  const QString &key)
{
  QPair<QString, bool> pair = _isArgLoggable_Python(
    key);
  return pair.second;
}

QString BaseRTValScriptableCommand_Python::getArg(
  const QString &key)
{
  QPair<QString, QString> pair = _getArg_Python(
    key);
  return pair.second;
}

void BaseRTValScriptableCommand_Python::setArg(
  const QString &key,
  const QString &value)
{
  _setArg_Python(
    key, 
    value);
}

void BaseRTValScriptableCommand_Python::validateSetArgs()
{
  _validateSetArgs_Python();
}

void BaseRTValScriptableCommand_Python::declareRTValArg(
  const QString &key, 
  const QString &type,
  bool optional, 
  RTVal defaultValue,
  bool loggable)
{
  _declareRTValArg_Python(
    key,
    type,
    optional,
    defaultValue,
    loggable);
}

QString BaseRTValScriptableCommand_Python::getRTValArgType(
  const QString &key)
{
  QPair<QString, QString> pair = _getRTValArgType_Python(
    key);
  return pair.second;
}

void BaseRTValScriptableCommand_Python::setRTValArgType(
  const QString &key,
  const QString &type)
{
 _setRTValArgType_Python(
    key,
    type);
}

RTVal BaseRTValScriptableCommand_Python::getRTValArg(
  const QString &key)
{
  QPair<QString, RTVal> pair = _getRTValArg_Python(
    key);
  return pair.second;
}

RTVal BaseRTValScriptableCommand_Python::getRTValArg(
  const QString &key,
  const QString &type)
{
  QPair<QString, RTVal> pair = _getRTValArg_Python(
    key,
    type);
  return pair.second;
}

void BaseRTValScriptableCommand_Python::setRTValArg(
  const QString &key,
  RTVal value)
{
  _setRTValArg_Python(
    key, 
    value);
}
