#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "ManipulationTool.h"
#include <FabricUI/Viewports/QtToKLEvent.h>
#include <map>
#include <iostream>
#include <FabricUI/Commands/KLCommandManager.h>

using namespace FabricUI::Viewports;


/////////////////////////////////////////////////////
// ManipulationCmd
FabricCore::RTVal ManipulationCmd::s_rtval_commands;

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
class ManipulationEventFilterObject : public QObject {
  public:
    ManipulationTool *tool;
    bool eventFilter(QObject *object, QEvent *event);
};

static ManipulationEventFilterObject sEventFilterObject;
ManipulationTool::ManipulationTool(GLViewportWidget * glView) : m_active(false), m_view(glView) {}

ManipulationTool::~ManipulationTool()
{
  ManipulationCmd::setStaticRTValCommands( FabricCore::RTVal() );
}

void ManipulationTool::toolOnSetup() {
  try
  {
    if(!m_eventDispatcher.isValid())
    {

      FabricCore::RTVal eventDispatcherHandle = FabricCore::RTVal::Create(m_view->getClient(), "EventDispatcherHandle", 0, 0);
      if(eventDispatcherHandle.isValid())
      {
        m_eventDispatcher = eventDispatcherHandle.callMethod("EventDispatcher", "getEventDispatcher", 0, 0);
      }
    }
    if(m_eventDispatcher.isValid())  m_eventDispatcher.callMethod("", "activateManipulation", 0, 0);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
    return;
  }

  m_active = true;
  //sEventFilterObject.tool = this;
  // m_view->installEventFilter(&sEventFilterObject);
  m_view->setFocus();
  m_view->setMouseTracking(true);
  m_view->updateGL();
}

void ManipulationTool::toolOffCleanup() {
  m_view->removeEventFilter(&sEventFilterObject);
  m_view->clearFocus();

  try
  {
    if(m_eventDispatcher.isValid())
      m_eventDispatcher.callMethod("", "deactivateManipulation", 0, 0);
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
    return;
  }
   
  m_active = false;
  m_view->setMouseTracking(false);
  m_view->updateGL();
  //m_eventDispatcher = FabricCore::RTVal();
}

bool ManipulationEventFilterObject::eventFilter(QObject *object, QEvent *event) {
  return tool->onEvent(event);
}

bool ManipulationTool::onEvent(QEvent *event) {
  if(!m_eventDispatcher.isValid())
    return false;

  // skip the alt key, so that we can continue to use the camera
  if(QApplication::keyboardModifiers().testFlag(Qt::AltModifier))
    return false;

  // Now we translate the Qt events to FabricEngine events..
  FabricCore::RTVal klevent = QtToKLEvent(event, m_view->getClient(), m_view->getViewport(), "Canvas");

  try
  {
    if(klevent.isValid())
    {
      //////////////////////////
      // Invoke the event...
      m_eventDispatcher.callMethod("Boolean", "onEvent", 1, &klevent);
      bool result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
      if(result) 
      {
        event->accept();
        Commands::KLCommandManager *manager = dynamic_cast<Commands::KLCommandManager *>(
          Commands::CommandManager::GetCommandManager());
        manager->synchronizeKL();
      }

      // In certain cases, the kl event is not accepted but should be.
      // We check if KL commands where added.
      if (event->type() == QEvent::MouseButtonRelease)
      {
        Commands::KLCommandManager *manager = dynamic_cast<Commands::KLCommandManager *>(
          Commands::CommandManager::GetCommandManager());
        manager->synchronizeKL();
      }

      FabricCore::RTVal host = klevent.maybeGetMember("host");
      if(host.maybeGetMember("redrawRequested").getBoolean())
        m_view->updateGL();

      // Cache the rtvals in a static variable that the command will then stor in the undo stack.
      if(host.callMethod("Boolean", "undoRedoCommandsAdded", 0, 0).getBoolean())
        ManipulationCmd::setStaticRTValCommands(host.callMethod("UndoRedoCommand[]", "getUndoRedoCommands", 0, 0));

      std::string customCommand = host.maybeGetMember("customCommand").getStringCString();
      if(customCommand == "setArg")
      {
        FabricCore::RTVal customCommandParams = host.maybeGetMember("customCommandParams");
        FabricCore::RTVal portNameVal = FabricCore::RTVal::ConstructString(m_view->getClient(), "portName");
        FabricCore::RTVal xfoVal = FabricCore::RTVal::ConstructString(m_view->getClient(), "xfo");
        std::string portName = customCommandParams.callMethod("String", "getString", 1, &portNameVal).getStringCString();
        m_lastManipValue = customCommandParams.callMethod("Xfo", "getXfo", 1, &xfoVal);
        if(portName.length() > 0)
          emit m_view->portManipulationRequested(portName.c_str());
      }

      klevent.invalidate();
      return result;
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Error: %s\n", e.getDesc_cstr());
    return false;
  }

  // the event was not handled by FabricEngine manipulation system. 
  return false;
}
