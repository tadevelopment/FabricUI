//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "KLCommandManager.h"
#include "KLCommandHelpers.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
 
KLCommandManager::KLCommandManager() 
  : RTValCommandManager()
{
  m_klCmdManager = GetKLCommandManager();
}

KLCommandManager::~KLCommandManager() 
{
}
 
void KLCommandManager::clear() 
{
  try 
  {
    m_klCmdManager.callMethod(
      "", 
      "clear", 
      0, 0);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::clear",
      "",
      e.getDesc_cstr());
  }
  
  CommandManager::clear();
}

QString KLCommandManager::getContent()
{
  QString res = CommandManager::getContent();

  try 
  {
    res += QString("\n") + m_klCmdManager.callMethod(
      "String", 
      "getContent", 
      0, 0).getStringCString();  
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::getContent",
      "",
      e.getDesc_cstr());
  }

  return res;
}
 
int KLCommandManager::getNewInteractionID()
{
  m_interactionIDCounter++;
  try
  {
    int interactionIDCounter = m_klCmdManager.callMethod(
      "UInt32", 
      "getInteractionIDCounter", 
      0, 0).getUInt32();

    if(interactionIDCounter < m_interactionIDCounter)
    {
      RTVal interactionIDCounterVal = RTVal::ConstructUInt32(
        m_klCmdManager.getContext(),
        m_interactionIDCounter);

      m_klCmdManager.callMethod(
        "", 
        "setInteractionIDCounter", 
        1, &interactionIDCounterVal);
    }

    else if(m_interactionIDCounter > interactionIDCounter)
      m_interactionIDCounter = interactionIDCounter;
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::getNewInteractionID",
      "",
      e.getDesc_cstr());
  }

  return m_interactionIDCounter;
}

void KLCommandManager::clearRedoStack() 
{
  try 
  {
    m_klCmdManager.callMethod(
      "", 
      "clearRedoStack", 
      0, 0);
      
    CommandManager::clearRedoStack();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::clearRedoStack",
      "",
      e.getDesc_cstr());
  }
}

void KLCommandManager::synchronizeKL() 
{
  try
  {
    // Gets the KL command from the KL manager. 
    RTVal klAppCmdStack = m_klCmdManager.callMethod(
      "Command[]", 
      "getAppStack", 
      0, 0);

    for(unsigned i=0; i<klAppCmdStack.getArraySize(); ++i)
    {
      // Gets the KL command from the KL manager. 
      RTVal klCmd = klAppCmdStack.getArrayElementRef(i);

      // Check if it's an AppCommand.
      // Construct C++ commands from KL
      RTVal appCmd = RTVal::Construct(
        klCmd.getContext(),
        "AppCommand", 
        1, 
        &klCmd);

      if(appCmd.isValid() && !appCmd.isNullObject())
        createAppCommand(appCmd);

      // KL commands have actually been 
      // created, create the C++ wrappers.
      else
        doKLCommand(klCmd);
    } 

    m_klCmdManager.callMethod(
      "", 
      "clearAppStack", 
      0, 0);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager::synchronizeKL",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "KLCommandManager::synchronizeKL",
      "",
      e.what());
  }
}

void KLCommandManager::createAppCommand(
  RTVal appCmd)
{
  RTVal keys = appCmd.callMethod(
    "String[]", 
    "getArgKeys", 
    0, 0);

  QMap<QString, RTVal> args;
  for(unsigned i=0; i<keys.getArraySize(); ++i)
  {
    RTVal argNameVal = keys.getArrayElement(i);
    args[argNameVal.getStringCString()] = appCmd.callMethod(
      "RTVal", 
      "getArg", 
      1, 
      &argNameVal);
  }

  createCommand(
    appCmd.callMethod("String", "getName", 0, 0).getStringCString(), 
    args, 
    true, 
    appCmd.callMethod("SInt32", "getInteractionID", 0, 0).getSInt32()
    ); 
}

void KLCommandManager::doKLCommand(
  RTVal klCmd)
{
  RTVal klScriptCmd = RTVal::Construct(
    klCmd.getContext(),
    "BaseScriptableCommand", 
    1, 
    &klCmd);

  BaseCommand *cmd = klScriptCmd.isValid() && !klScriptCmd.isNullObject()
    ? (BaseCommand *)new KLScriptableCommand(klScriptCmd)
    : (BaseCommand *)new KLCommand(klCmd);

  doCommand(cmd);
}
