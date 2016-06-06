/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGLRenderer.h"
#include <FabricUI/Viewports/QtToKLEvent.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <iostream>
#include <fstream>

using namespace std;
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
      driveNodeInputPorts(controller, args[0]);
 
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
    RTVal args[4] = {
      RTVal::ConstructVariableArray(m_client, "String"),
      RTVal::ConstructVariableArray(m_client, "ToolType"),
      RTVal::ConstructVariableArray(m_client, "Key"),
      RTVal::ConstructVariableArray(m_client, "Boolean")
    };
    m_shGLRendererVal.callMethod("", "getRegisteredTools", 4, args);

    QStringList toolNames, toolTypes, toolKeys, isEnables;
    for(unsigned int i=0; i<args[0].getArraySize(); ++i)
    {
      QString name(args[0].getArrayElement(i).getStringCString());
      unsigned int type(args[1].getArrayElement(i).getUInt32());
      QString typeStr = type == 0 ? "Shared" : 
                        type == 1 ? "Mutually Exclusive" : 
                        type == 2 ? "Fully Exclusive" : "Independent";
      QString key(QKeySequence(args[2].getArrayElement(i).getUInt32()).toString());
      QString isEnable = QString::number(args[3].getArrayElement(i).getBoolean());
      
      toolNames.append(name);
      toolTypes.append(typeStr);
      toolKeys.append(key);
      isEnables.append(isEnable);
    }
    list.append(toolNames);
    list.append(toolTypes);
    list.append(toolKeys);
    list.append(isEnables);
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

void SHGLRenderer::driveNodeInputPorts(
  DFG::DFGController *controller,
  RTVal event) 
{
  try 
  {
    QString toolPath(event.maybeGetMember("dfgToolPath").getStringCString());    

    if(toolPath != "") 
    {
      DFGBinding binding = controller->getBinding();
      DFGExec exec = binding.getExec();

      if(exec.hasVar(toolPath.toUtf8().constData()))
      {
        QStringList list = toolPath.split(".");
        QString contenerPath = list[list.size() - 2];

        QString subExecParentPath;
        for(int i=0; i<list.size()-2; ++i) 
          subExecParentPath += list[i] + ".";
        subExecParentPath.left(subExecParentPath.lastIndexOf("."));
        if(subExecParentPath == "") 
          subExecParentPath = ".";

        QString subExecPath = toolPath.left(toolPath.lastIndexOf("."));
        DFGExec subExec = exec.getSubExec(subExecPath.toUtf8().constData());
        DFGExec subExecParent = exec.getSubExec(subExecParentPath.toUtf8().constData());
 
        RTVal toolVal = exec.getVarValue(toolPath.toUtf8().constData());
        RTVal target = toolVal.callMethod("DFGToolTarget", "getTarget", 0, 0);
        RTVal toolData = target.callMethod("DFGToolData", "getToolData", 0, 0);

        bool bakeValue = toolData.callMethod("Boolean", "bakeValue", 0, 0).getBoolean();  
        int portCount = toolData.callMethod("UInt32", "getPortCount", 0, 0).getUInt32();
        
        for(int i=0; i<portCount; ++i)
        {
          RTVal index = RTVal::ConstructUInt32(m_client, i);
          QString portName(toolData.callMethod("String", "getPortAtIndex", 1, &index).getStringCString()); 
          QString portPath = contenerPath + "." + portName;

          if(subExec.haveExecPort(portName.toUtf8().constData()))
          {
            if(subExec.getExecPortType(portName.toUtf8().constData()) == DFGPortType_In)
            {
              RTVal args[2] = { index, RTVal::ConstructBoolean(m_client, false) };
              RTVal rtVal = toolData.callMethod("RTVal", "getRTValAtPortIndex", 2, args);
              QString valType(rtVal.callMethod("String", "type", 0, 0).getStringCString());

              unsigned int portIndexInExec = subExec.getExecPortIndex(portName.toUtf8().constData());
              if(subExec.isExecPortResolvedType(portIndexInExec, valType.toUtf8().constData()))   
              {
                RTVal val = RTVal::Construct(m_client, valType.toUtf8().constData(), 1, &rtVal);

                if(!bakeValue)
                  subExecParent.setPortDefaultValue(portPath.toUtf8().constData(), val, false);
                else
                {
                  args[1] = RTVal::ConstructBoolean(m_client, true);
                  RTVal prevRTVal = toolData.callMethod("RTVal", "getRTValAtPortIndex", 2, args);
                  RTVal prevVal = RTVal::Construct(m_client, valType.toUtf8().constData(), 1, &prevRTVal);

                  subExecParent.setPortDefaultValue(portPath.toUtf8().constData(), prevVal, false);
                  controller->getCmdHandler()->dfgDoSetPortDefaultValue(binding, subExecParentPath, subExecParent, portPath, val);
                }
              }

              else
              {
                QString portType(subExec.getExecPortResolvedType(portName.toUtf8().constData()));
                printf("SHGLRenderer::driveNodeInputPorts: Warning : "
                  "The port: %s 's type %s doesn't match with the target manipulated type %s\n", 
                  portName.toUtf8().constData(), 
                  portType.toUtf8().constData(),
                  valType.toUtf8().constData());
              }
            }

            else
            {
              printf("SHGLRenderer::driveNodeInputPorts: Warning: "
                "The driven port is not an InputPort\n");
            }
          }

          else
          {
            printf("SHGLRenderer::driveNodeInputPorts: Warning : "
              "The port: %s doesn't exist\n", portName.toUtf8().constData());        
          }
        }
      }
      else
      {
        printf("SHGLRenderer::driveNodeInputPorts: Warning : "
          "The tool: %s is not a variable\n", toolPath.toUtf8().constData());    
      }
    }
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::driveNodeInputPorts: exception: %s\n", e.getDesc_cstr());
  }
}
