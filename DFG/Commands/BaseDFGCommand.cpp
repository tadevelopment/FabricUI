// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "BaseDFGCommand.h"

using namespace FabricUI;
using namespace DFG;
using namespace Commands;
using namespace FabricCore;

BaseDFGCommand::BaseDFGCommand() 
  : BaseRTValScriptableCommand()
  , m_coreCmdCount(0)
  , m_dfgController(0)
{
}

BaseDFGCommand::~BaseDFGCommand() 
{
}

void BaseDFGCommand::registrationCallback(
  const QString &name, 
  void *userData)
{
  BaseScriptableCommand::registrationCallback(
    name,
    userData);

  m_dfgController = (DFGController*)userData;

  if(m_dfgController == 0)
    throw(
      "BaseDFGCommand::registrationCallback, error: DFGController is null"
    );
}

bool BaseDFGCommand::canUndo()
{
  return m_coreCmdCount > 0;
}

bool BaseDFGCommand::undoIt()
{   
  try 
  {
    for (unsigned i = 0; i < m_coreCmdCount; ++i)
      m_dfgController->getBinding().getHost().maybeUndo();

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "BaseDFGCommand::undoIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
}

bool BaseDFGCommand::redoIt()
{
  try 
  {
    for (unsigned i = 0; i < m_coreCmdCount; ++i)
      m_dfgController->getBinding().getHost().maybeRedo();

    return true;
  }

  catch(Exception &e)
  {
    printf(
      "BaseDFGCommand::redoIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return false;
}
