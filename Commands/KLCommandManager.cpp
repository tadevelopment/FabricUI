//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
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
  std::cout 
    << "KLCommandManager::KLCommandManager " 
    << std::endl;

 // m_klCmdManager = KLCommandHelpers::getKLCommandManager();
}

KLCommandManager::~KLCommandManager() 
{
}
 
void KLCommandManager::clear() 
{
  FABRIC_CATCH_BEGIN();

  KLCommandHelpers::getKLCommandManager().callMethod(
    "", 
    "clear", 
    0, 0);

  CommandManager::clear();

  FABRIC_CATCH_END("KLCommandManager::clear");
}

QString KLCommandManager::getContent(
  bool withArgs)
{
  FABRIC_CATCH_BEGIN();

  QString res = CommandManager::getContent(
    withArgs);

  res += QString("\n") + KLCommandHelpers::getKLCommandManager().callMethod(
    "String", 
    "getContent", 
    0, 0).getStringCString();  
  
  return res;

  FABRIC_CATCH_END("KLCommandManager::getContent");

  return "";
}
 
int KLCommandManager::getNewCanMergeID()
{
  m_canMergeIDCounter++;
  
  FABRIC_CATCH_BEGIN();

  int canMergeIDCounter = KLCommandHelpers::getKLCommandManager().callMethod(
    "UInt32", 
    "getCanMergeIDCounter", 
    0, 0).getUInt32();

  if(canMergeIDCounter < m_canMergeIDCounter)
  {
    RTVal canMergeIDCounterVal = RTVal::ConstructUInt32(
      KLCommandHelpers::getKLCommandManager().getContext(),
      m_canMergeIDCounter);

    KLCommandHelpers::getKLCommandManager().callMethod(
      "", 
      "setCanMergeIDCounter", 
      1, &canMergeIDCounterVal);
  }

  else if(m_canMergeIDCounter > canMergeIDCounter)
    m_canMergeIDCounter = canMergeIDCounter;

  FABRIC_CATCH_END("KLCommandManager::getNewCanMergeID");

  return m_canMergeIDCounter;
}

void KLCommandManager::clearRedoStack() 
{
  FABRIC_CATCH_BEGIN();

  KLCommandHelpers::getKLCommandManager().callMethod(
    "", 
    "clearRedoStack", 
    0, 0);
    
  CommandManager::clearRedoStack();

  FABRIC_CATCH_END("KLCommandManager::clearRedoStack");
}

void KLCommandManager::synchronizeKL() 
{
  FABRIC_CATCH_BEGIN();

  // Gets the KL command from the KL manager. 
  RTVal klAppCmdStack = KLCommandHelpers::getKLCommandManager().callMethod(
    "Command[]", 
    "getAppStack", 
    0, 0);
  std::cout << "KLCommandManager::synchronizeKL " << klAppCmdStack.getArraySize() << std::endl;

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

  KLCommandHelpers::getKLCommandManager().callMethod(
    "", 
    "clearAppStack", 
    0, 0);

  FABRIC_CATCH_END("KLCommandManager::synchronizeKL");
}

void KLCommandManager::createAppCommand(
  RTVal appCmd)
{
  FABRIC_CATCH_BEGIN();

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

  std::cout << "KLCommandManager::createAppCommand " << appCmd.callMethod("String", "getName", 0, 0).getStringCString() << std::endl;

  createCommand(
    appCmd.callMethod("String", "getName", 0, 0).getStringCString(), 
    args, 
    true, 
    appCmd.callMethod("SInt32", "getCanMergeID", 0, 0).getSInt32()
    ); 

  FABRIC_CATCH_END("KLCommandManager::createAppCommand");
}

void KLCommandManager::doKLCommand(
  RTVal klCmd)
{
  FABRIC_CATCH_BEGIN();

  RTVal klScriptCmd = RTVal::Construct(
    klCmd.getContext(),
    "BaseScriptableCommand", 
    1, 
    &klCmd);

  BaseCommand *cmd = 0;
  if(klScriptCmd.isValid() && !klScriptCmd.isNullObject())
    cmd = new KLScriptableCommand(klScriptCmd);
  else
    cmd = new KLCommand(klCmd);

  RTVal baseCmd = RTVal::Construct(
    klCmd.getContext(),
    "BaseCommand", 
    1, 
    &klCmd);

  doCommand(
    cmd,
    baseCmd.callMethod("SInt32", "getCanMergeID", 0, 0).getSInt32()
    );

  FABRIC_CATCH_END("KLCommandManager::doKLCommand");
}
