/*
*  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
*/

#include <QObject>
#include <QApplication>
#include "ManipulationTool.h"
#include <FabricUI/Viewports/QtToKLEvent.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Viewports;
using namespace FabricCore;
using namespace Application;


/////////////////////////////////////////////////////
// ManipulationCmd
RTVal ManipulationCmd::s_rtval_commands;

ManipulationCmd::ManipulationCmd() {
  m_rtval_commands = s_rtval_commands;
}

ManipulationCmd::~ManipulationCmd() {}

bool ManipulationCmd::invoke() {
  if(m_rtval_commands.isValid())
  {
    for(uint32_t i=0; i<m_rtval_commands.getArraySize(); i++)
      m_rtval_commands.getArrayElement(i).callMethod("", "doAction", 0, 0);
  }
  return true;
}

bool ManipulationCmd::undo() {
  if(m_rtval_commands.isValid())
  {
    for(uint32_t i=0; i<m_rtval_commands.getArraySize(); i++)
      m_rtval_commands.getArrayElement(i).callMethod("", "undoAction", 0, 0);
  }
  return true;
}


/////////////////////////////////////////////////////
// ManipulationTool
ManipulationTool::ManipulationTool() : m_active(false) {}

ManipulationTool::~ManipulationTool()
{
  ManipulationCmd::setStaticRTValCommands( RTVal() );
}

void ManipulationTool::toolOnSetup() {
  try
  {
    if(!m_eventDispatcher.isValid())
    {
      RTVal eventDispatcherHandle = RTVal::Create(FabricApplicationStates::GetAppStates()->getClient(), "EventDispatcherHandle", 0, 0);
      if(eventDispatcherHandle.isValid())
        m_eventDispatcher = eventDispatcherHandle.callMethod("EventDispatcher", "getEventDispatcher", 0, 0);
    }
    if(m_eventDispatcher.isValid())  
      m_eventDispatcher.callMethod("", "activateManipulation", 0, 0);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "ManipulationTool::toolOnSetup",
      "",
      e.getDesc_cstr());
  }

  m_active = true;
}

void ManipulationTool::toolOffCleanup() {

  try
  {
    if(m_eventDispatcher.isValid())
      m_eventDispatcher.callMethod("", "deactivateManipulation", 0, 0);
  }

  catch(Exception e)
  {
     FabricException::Throw(
      "ManipulationTool::toolOffCleanup",
      "",
      e.getDesc_cstr());
  }
   
  m_active = false;
  m_eventDispatcher = RTVal();
}

bool ManipulationTool::onEvent(QEvent *event, RTVal viewport, bool &redrawRequested, QString &manipulatedPortName) {
    
  redrawRequested = false;
  manipulatedPortName = "";

  if(!m_eventDispatcher.isValid())
    return false;

  // Now we translate the Qt events to FabricEngine events.
  try
  {
    Client client = FabricApplicationStates::GetAppStates()->getClient();
    RTVal klevent = QtToKLEvent(event, client, viewport, "Canvas");

    if(klevent.isValid())
    {
      //////////////////////////
      // Invoke the event...
      m_eventDispatcher.callMethod("Boolean", "onEvent", 1, &klevent);
      bool result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
      if(result) 
        event->accept();
       
      // In certain cases, the kl event is not accepted but should be.
      // We check if KL commands where added.
      if (result || event->type() == QEvent::MouseButtonRelease)
      {
        Commands::KLCommandManager *manager = qobject_cast<Commands::KLCommandManager *>(
          Commands::CommandManager::GetCommandManager());
        manager->synchronizeKL();
      }     

      RTVal host = klevent.maybeGetMember("host");
      redrawRequested = host.maybeGetMember("redrawRequested").getBoolean();
 
      // Cache the rtvals in a static variable that the command will then stor in the undo stack.
      if(host.callMethod("Boolean", "undoRedoCommandsAdded", 0, 0).getBoolean())
        ManipulationCmd::setStaticRTValCommands(host.callMethod("UndoRedoCommand[]", "getUndoRedoCommands", 0, 0));

      std::string customCommand = host.maybeGetMember("customCommand").getStringCString();
      if(customCommand == "setArg")
      {
        RTVal customCommandParams = host.maybeGetMember("customCommandParams");
        RTVal portNameVal = RTVal::ConstructString(client, "portName");
        RTVal xfoVal = RTVal::ConstructString(client, "xfo");
        manipulatedPortName = customCommandParams.callMethod("String", "getString", 1, &portNameVal).getStringCString();
        m_lastManipValue = customCommandParams.callMethod("Xfo", "getXfo", 1, &xfoVal);
      }

      return result;
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "ManipulationTool::onEvent",
      "",
      e.getDesc_cstr());
  }

  // the event was not handled by FabricEngine manipulation system. 
  return false;
}
