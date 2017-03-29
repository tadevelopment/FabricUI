//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "CommandManager.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

KLScriptableCommand::KLScriptableCommand(
  RTVal klCmd)
  : BaseRTValScriptableCommand() 
  , m_klCmd(klCmd)
{
}

KLScriptableCommand::~KLScriptableCommand() 
{
}

QString KLScriptableCommand::getName() 
{
  return KLCommand::GetName(m_klCmd);
}

bool KLScriptableCommand::canUndo() 
{
  return KLCommand::CanUndo(m_klCmd);
}

bool KLScriptableCommand::doIt() 
{ 
  return KLCommand::DoIt(
    m_klCmd.callMethod(
      "Command", 
      "getAsCommand", 
      0, 
      0));
}

bool KLScriptableCommand::undoIt() 
{ 
  return KLCommand::UndoIt();
}

bool KLScriptableCommand::redoIt() 
{  
  return KLCommand::RedoIt();
}
 
QString KLScriptableCommand::getArg(
  const QString &key) 
{
  return Util::RTValUtil::klRTValToJSON(
    getArgAsRTVal(
      key)
    );
}

bool KLScriptableCommand::hasArg(
  const QString &key) 
{
  bool res = false;

  try 
  {
    RTVal keyVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      key.toUtf8().constData());

    res = m_klCmd.callMethod(
      "Boolean",
      "hasArg",
      1,
      &keyVal
      ).getBoolean();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::hasArg: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

QMap<QString, QString> KLScriptableCommand::getArgs() 
{
  QMap<QString, QString> res;
  
  try 
  {
    RTVal argNameList = m_klCmd.callMethod(
      "String[]", 
      "getArgNameList", 
      0, 
      0);

    for(unsigned int i=0; i<argNameList.getArraySize(); ++i)
    {
      QString key = argNameList.getArrayElement(i).getStringCString();
      res[key] = getArg(key);
    }
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getArgs: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}
 
void KLScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  if(key.isEmpty()) 
    throw(
      std::string("KLScriptableCommand::setArg, error setting arg: key not specified")
    );

  QString strError;

  try 
  {
    // Convert the C++ RTVal wrapping the KL arg to a C++ 
    // RTVal wrapping a KL RTVal wrapping the KL arg :)
    RTVal keyVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      key.toUtf8().constData());

    // Get the type of the arg stored by the KL RTVal.
    QString rtValType = m_klCmd.callMethod(
      "String",
      "getArgType",
      1,
      &keyVal
      ).getStringCString();

    // Get the argument from its JSON description.
    RTVal klRTVal = Util::RTValUtil::jsonToKLRTVal(
      CommandManager::GetCommandManager()->getFabricClient(),
      json,
      rtValType);

    // Set the KL command arguments.
    RTVal args[3] = { 
      keyVal, 
      klRTVal, 
      RTVal::ConstructString(
        CommandManager::GetCommandManager()->getFabricClient(), 
        "") 
    };

    m_klCmd.callMethod(
      "", 
      "setArg", 
      3, 
      args);

    // Gets possible KL errors.
    strError = args[2].getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::setArg: exception: %s\n", 
      e.getDesc_cstr());
  }

  if(!strError.isEmpty())
    throw(
      std::string(
        strError.toUtf8().constData()
      )
    );
}

QString KLScriptableCommand::getArgType(
  const QString &key)
{
  QString res;

  try 
  {
    RTVal keyVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      key.toUtf8().constData());

    res = m_klCmd.callMethod(
      "String", 
      "getArgType", 
      1, 
      &keyVal
      ).getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getArgType: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return res;
}

void KLScriptableCommand::setArgType(
  const QString &key, 
  const QString &type) 
{
  // Does nothing
}

RTVal KLScriptableCommand::getArgAsRTVal( 
  const QString &key)
{
  RTVal klRTVal;

  if(key.isEmpty()) 
    throw(
      std::string(
        "KLScriptableCommand::getArgAsRTVal, error getting arg: key not specified")
    );

  try 
  {
    RTVal keyVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      key.toUtf8().constData());

    klRTVal = m_klCmd.callMethod(
      "RTVal", 
      "getArg", 
      1, 
      &keyVal);
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getArgAsRTVal: exception: %s\n", 
      e.getDesc_cstr());
  }

  return klRTVal;
}

void KLScriptableCommand::validateSetArgs()
{
  QString strError;

  try 
  {
    RTVal errorVal = RTVal::ConstructString(
      CommandManager::GetCommandManager()->getFabricClient(), 
      "");

    m_klCmd.callMethod(
      "", 
      "validateSetArgs", 
      1, 
      &errorVal);

    strError = errorVal.getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::validateSetArgs: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  if(!strError.isEmpty())
     throw(
      std::string(
        strError.toUtf8().constData()
      )
    );
}

QString KLScriptableCommand::getArgsDescription() 
{ 
  try 
  {
    return m_klCmd.callMethod(
      "String",
      "getArgsDescription", 
      0, 
      0).getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getArgsDescription: exception: %s\n", 
      e.getDesc_cstr());
  }

  return "";
}

QString KLScriptableCommand::getHelp() 
{
  return KLCommand::GetHelp(m_klCmd);
}
