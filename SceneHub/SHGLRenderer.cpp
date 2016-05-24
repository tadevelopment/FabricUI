/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include "SHGLRenderer.h"
#include <FabricUI/Viewports/QtToKLEvent.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;

SHGLRenderer::SHGLRenderer(Client client) 
  : m_client(client) {
  try 
  {
    RTVal dummyGLRendererVal = RTVal::Construct(m_client, "SHGLRenderer", 0, 0);
    m_shGLRendererVal = dummyGLRendererVal.callMethod("SHRenderer", "create", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::SHGLRenderer: exception: %s\n", e.getDesc_cstr());
  }
}

SHGLRenderer::SHGLRenderer(Client client, RTVal shRenderer) 
  : m_client(client)
  , m_shGLRendererVal(shRenderer) {
}

Client SHGLRenderer::getClient() { 
  return m_client; 
}

RTVal SHGLRenderer::getSHGLRenderer() { 
  return m_shGLRendererVal; 
}

void SHGLRenderer::update() {
  try 
  {
    RTVal dummyGLRendererVal = RTVal::Construct(m_client, "SHGLRenderer", 0, 0);
    m_shGLRendererVal = dummyGLRendererVal.callMethod("SHRenderer", "getOrCreate", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::update: exception: %s\n", e.getDesc_cstr());
  }
}

QList<unsigned int> SHGLRenderer::getDrawStats(unsigned int viewportID) {
  QList<unsigned int> stats;
  try 
  {
    RTVal args[5] = {
      RTVal::ConstructUInt32(m_client, viewportID), 
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0)
    };
    m_shGLRendererVal.callMethod("Boolean", "getDrawStats", 5, &args[0]);
    stats.append(args[1].getUInt32());
    stats.append(args[2].getUInt32());
    stats.append(args[3].getUInt32());
    stats.append(args[4].getUInt32());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getDrawStats: exception: %s\n", e.getDesc_cstr());
  }
  return stats;
}

RTVal SHGLRenderer::getViewport(unsigned int viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

RTVal SHGLRenderer::getOrAddViewport(unsigned int viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getOrAddViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

RTVal SHGLRenderer::getOrAddStereoViewport(unsigned int viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getOrAddStereoViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddStereoViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

void SHGLRenderer::removeViewport(unsigned int viewportID) {
  try 
  {
    RTVal glRenderer = m_shGLRendererVal;
    if(glRenderer.isValid() && !glRenderer.isNullObject()) {
      RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
      m_shGLRendererVal.callMethod("BaseRTRViewport", "removeViewport", 1, &arg);
    }
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::removeViewport: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::setOrthographicViewport(unsigned int viewportID, bool orthographic) {
  try 
  {
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructBoolean(m_client, orthographic)
    };
    m_shGLRendererVal.callMethod("", "setOrthographicViewport", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::setOrthographicViewport: exception: %s\n", e.getDesc_cstr());
  }
}

RTVal SHGLRenderer::getCamera(unsigned int viewportID) {
  RTVal cameraVal;
  try 
  {
    RTVal viewportVal = getOrAddViewport(viewportID);
    cameraVal = viewportVal.callMethod("Camera", "getCamera", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddStereoViewport: exception: %s\n", e.getDesc_cstr());
  }
  return cameraVal;
}

void SHGLRenderer::setPlayback(bool playback) {
  try 
  {
    RTVal arg = RTVal::ConstructBoolean(m_client, playback);
    m_shGLRendererVal.callMethod("", "setPlayback", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::setPlayback: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLRenderer::isPlayback() {
  try 
  {
    return m_shGLRendererVal.callMethod("Boolean", "isPlayback", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::isPlayback: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

RTVal SHGLRenderer::castRay(unsigned int viewportID, QPoint pos) {
  RTVal rayVal;
  try 
  {
    RTVal posVal = RTVal::Construct(m_client, "Vec2", 0, 0);
    posVal.setMember("x", RTVal::ConstructFloat32(m_client, pos.x()));
    posVal.setMember("y", RTVal::ConstructFloat32(m_client, pos.y()));
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal
    };
    rayVal = m_shGLRendererVal.callMethod("Ray", "castRay", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::castRay: exception: %s\n", e.getDesc_cstr());
  }
  return rayVal;
}

QList<float> SHGLRenderer::get3DScenePosFrom2DScreenPos(unsigned int viewportID, QPoint pos) {
  QList<float> list;
  try 
  {
    RTVal posVal = QtToKLMousePosition(pos, m_client, getOrAddViewport(viewportID), true);
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal
    };
    RTVal pos3DVal = m_shGLRendererVal.callMethod("Vec3", "get3DScenePosFrom2DScreenPos", 2, &args[0]);
    list.append(pos3DVal.maybeGetMember("x").getFloat32());
    list.append(pos3DVal.maybeGetMember("y").getFloat32());
    list.append(pos3DVal.maybeGetMember("z").getFloat32());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::get3DScenePosFrom2DScreenPos: exception: %s\n", e.getDesc_cstr());
  }
  return list;
}

RTVal SHGLRenderer::getSGObjectFrom2DScreenPos(unsigned int viewportID, QPoint pos) {
  RTVal result;
  try {
    RTVal posVal = QtToKLMousePosition(pos, m_client, getOrAddViewport(viewportID), true);
    RTVal validVal = RTVal::ConstructBoolean(m_client, false);
    RTVal args[3] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal,
      validVal
    };
    result = m_shGLRendererVal.callMethod("SGObject", "getSGObjectFrom2DScreenPos", 3, args);
    if(!validVal.getBoolean())
      result = RTVal();//set as empty
  }
  catch(Exception e) {
    printf("SHGLRenderer::getSGObjectFrom2DScreenPos: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

void SHGLRenderer::render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples) {
  try 
  {
    RTVal args[4] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructUInt32(m_client, width),
      RTVal::ConstructUInt32(m_client, height),
      RTVal::ConstructUInt32(m_client, samples)
    };
    m_shGLRendererVal.callMethod("", "render", 4, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::render: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples, unsigned int drawPhase) {
  try 
  {
    RTVal args[5] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructUInt32(m_client, width),
      RTVal::ConstructUInt32(m_client, height),
      RTVal::ConstructUInt32(m_client, samples),
      RTVal::ConstructUInt32(m_client, drawPhase)
    };
    m_shGLRendererVal.callMethod("", "render", 5, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::render: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLRenderer::onEvent(
  unsigned int viewportID, 
  QEvent *event, 
  bool &redrawAllViewports, 
  bool dragging,
  DFG::DFGController *controller)
{
  try 
  {
    RTVal viewportVal = getOrAddViewport(viewportID);

    RTVal args[2] = {
      QtToKLEvent(event, m_client, viewportVal, true),
      RTVal::ConstructBoolean(m_client, dragging)
    };
    
    m_shGLRendererVal.callMethod("", "onEvent", 2, args);
    bool result = args[0].callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
    event->setAccepted(result);
    redrawAllViewports = args[0].callMethod("Boolean", "redrawAllViewports", 0, 0).getBoolean();

    
    if(controller && result)
    {
      //std::cerr << "I am here 1 " << std::endl;
      DFGExec exec = controller->getBinding().getExec();

      RTVal dfgHost = args[0].callMethod("DFGHost", "getHost", 0, 0);
      if(!dfgHost.isNullObject())
      {
        int numberPorts = dfgHost.callMethod("UInt32", "getNumberOfPorts", 0, 0).getUInt32();
        QString toolName = QString(dfgHost.callMethod("String", "getToolName", 0, 0).getStringCString());    
        std::cerr << "numberPorts " << numberPorts << std::endl;
        std::cerr << "toolName " << toolName.toStdString() << std::endl;

        for(int i=0; i<numberPorts; ++i)
        {
          RTVal portIndex = RTVal::ConstructUInt32(m_client, i);
          QString portPath = QString(dfgHost.callMethod("String", "getPortPath", 1, &portIndex).getStringCString());
          QString subExecPath = portPath.left(portPath.lastIndexOf("."));
          QString portName = portPath.mid(portPath.lastIndexOf(".")+1);
            
          std::cerr << "portPath[" << i << "] "    << portPath.toStdString()    << std::endl;
          std::cerr << "subExecPath[" << i << "] " << subExecPath.toStdString() << std::endl;
          std::cerr << "portName[" << i << "] "    << portName.toStdString()    << std::endl;
          
          DFGExec subExec = exec.getSubExec(subExecPath.toUtf8().constData());
          QString portType(subExec.getExecPortResolvedType(portName.toUtf8().constData()));

          if( subExec.haveExecPort(portName.toUtf8().constData()) )
          {            
            std::cerr << "I am here 3 " << std::endl;
            RTVal resVal = RTVal::Construct(m_client, portType.toUtf8().constData(), 0, 0);
            //RTVal dataVal = resVal.callMethod("Data", "data", 0, 0);
            //dfgHost.callMethod("", "getData", 1, &dataVal);
            subExec.setPortDefaultValue(portName.toUtf8().constData(), resVal);
            //controller->getBinding().setArgValue(portPath.toUtf8().constData(), resVal, false);
          }
        }
      }
    }
 
    return result;
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::onEvent: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

RTVal SHGLRenderer::getToolDispatcher() {
  RTVal toolDispatcherVal;
  try 
  {
    toolDispatcherVal = m_shGLRendererVal.callMethod("RTRToolDispatcher", "getToolDispatcher", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getToolDispatcher: exception: %s\n", e.getDesc_cstr());
  }
  return toolDispatcherVal;
}

QList<QStringList> SHGLRenderer::getRegisteredTools() {
  QList<QStringList> list;
  try 
  {
    RTVal args[2] = {
      RTVal::ConstructVariableArray(m_client, "String"),
      RTVal::ConstructVariableArray(m_client, "Key")
    };
    m_shGLRendererVal.callMethod("", "getRegisteredTools", 2, &args[0]);

    QStringList toolNames;
    QStringList toolKeys;
    for(unsigned int i=0; i<args[0].getArraySize(); ++i)
    {
      RTVal name = args[0].getArrayElement(i);
      RTVal key = args[1].getArrayElement(i);
      toolNames.append(name.getStringCString());
      toolKeys.append(QKeySequence(key.getUInt32()).toString());
    }
    list.append(toolNames);
    list.append(toolKeys);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getRegisteredTools: exception: %s\n", e.getDesc_cstr());
  }
  return list;
}

RTVal SHGLRenderer::getSelectionSet() {
  RTVal selectionSetVal;
  try 
  {
    selectionSetVal = m_shGLRendererVal.callMethod("SWElementHandleSet", "getSelectionSet", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getSelectionSet: exception: %s\n", e.getDesc_cstr());
  }
  return selectionSetVal;
}

bool SHGLRenderer::selectionChangedFromManips() {
  try 
  {
    return m_shGLRendererVal.callMethod("Boolean", "selectionChangedFromManips", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::selectionChangedFromManips: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

QString SHGLRenderer::getSelectionCategory() {
  try 
  {
    return QString(m_shGLRendererVal.callMethod("String", "getSelectionCategory", 0, 0).getStringCString());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return QString();
}
