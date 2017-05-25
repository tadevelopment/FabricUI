//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "BaseRTValScriptableCommand_Python.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

BaseRTValScriptableCommand_Python::BaseRTValScriptableCommand_Python() 
  : BaseRTValScriptableCommand() 
{
}

BaseRTValScriptableCommand_Python::~BaseRTValScriptableCommand_Python() 
{
}

QString BaseRTValScriptableCommand_Python::_declareArg_Python(
  const QString &key, 
  int flag, 
  const QString &defaultValue)
{
  QString error;

  try
  {
    BaseRTValScriptableCommand::declareArg(
      key,
      flag,
      defaultValue);
  }

  catch(FabricException &e) 
  {
    error = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_declareArg_Python",
      "",
      e.what(),
      NOTHING);
  }

  return error;
}

QPair<QString, bool> BaseRTValScriptableCommand_Python::_isArg_Python(
  const QString &key,
  int flag)
{
  QPair<QString, bool> pair;
    
  try
  {
    pair.second = BaseRTValScriptableCommand::isArg(key, flag);
  }

  catch(FabricException &e) 
  {
    pair.first = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_isArg_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    pair.first = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_getArg_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    error = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_setArg_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    error = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_validateSetArgs_Python",
      "",
      e.what(),
      NOTHING);
  }
 
  return error;
}
  
QString BaseRTValScriptableCommand_Python::_declareRTValArg_Python(
  const QString &key, 
  const QString &type,
  int flag, 
  RTVal defaultValue)
{
  QString error;

  try
  {
    BaseRTValScriptableCommand::declareRTValArg(
      key,
      type,
      flag,
      defaultValue);
  }

  catch(FabricException &e) 
  {
    error = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_declareRTValArg_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    pair.first = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_getRTValArgType_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    error = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_setRTValArgType_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    pair.first = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_getRTValArg_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    pair.first = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_getRTValArg_Python",
      "",
      e.what(),
      NOTHING);
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

  catch(FabricException &e) 
  {
    error = FabricException::Throw(
      "BaseRTValScriptableCommand_Python::_setRTValArg_Python",
      "",
      e.what(),
      NOTHING);
  }
 
  return error;
} 

void BaseRTValScriptableCommand_Python::declareArg(
  const QString &key, 
  int flag, 
  const QString &defaultValue)
{
  _declareArg_Python(
    key,
    flag,
    defaultValue);
}

bool BaseRTValScriptableCommand_Python::isArg(
  const QString &key,
  int flag)
{
  QPair<QString, bool> pair = _isArg_Python(
    key, flag);
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
  int flag, 
  RTVal defaultValue)
{
  _declareRTValArg_Python(
    key,
    type,
    flag,
    defaultValue);
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
