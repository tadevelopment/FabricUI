//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
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

QString BaseScriptableCommand_Python::declareArg_Python(
  const QString &key, 
  bool optional, 
  const QString &defaultValue) 
{
  QString error;
  try 
  { 
    BaseScriptableCommand::declareArg(key, optional, defaultValue);
  }  
  catch (std::string &e) 
  {
    error = e.c_str(); 
  }
  return error;
}

QString BaseScriptableCommand_Python::setArg_Python(
  const QString &key, 
  const QString &value) 
{
  QString error;
  try 
  { 
    BaseScriptableCommand::setArg(key, value);
  }  
  catch (std::string &e) 
  {
    error = e.c_str(); 
  }
  return error;
}

QString BaseScriptableCommand_Python::validateSetArgs_Python() const
{
  QString error;
  try 
  { 
    BaseScriptableCommand::validateSetArgs();
  }  
  catch (std::string &e) 
  {
    error = e.c_str(); 
  }
  return error;
}
