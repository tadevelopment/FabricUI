/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGLScene.h"
#include <FabricUI/Viewports/QtToKLEvent.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;

inline QString loadScene(Client &client, QString const &klFile) {
  FILE * klFilePtr = fopen(klFile.toUtf8().constData(), "rb");
  if(!klFilePtr) 
  {
    printf("KL file '%s' not found.", klFile.toUtf8().constData());
    exit(1);//close();
    return "";
  }

  fseek( klFilePtr, 0, SEEK_END );
  int fileSize = ftell( klFilePtr );
  rewind( klFilePtr );

  char * klCodeBuffer = (char*) malloc(fileSize + 1);
  klCodeBuffer[fileSize] = '\0';

  fread(klCodeBuffer, fileSize, 1, klFilePtr);
  fclose(klFilePtr);

  std::string klCode = klCodeBuffer;
  free(klCodeBuffer);

  QString prefix = QFile(klFile).fileName();
  prefix = prefix.replace('\\', '/');
  QStringList prefixParts = prefix.split('/');
  prefix = prefixParts[prefixParts.length()-1];
  prefix = prefix.split('.')[0];

  KLSourceFile sourceFile;
  sourceFile.filenameCStr = klFile.toUtf8().constData();
  sourceFile.sourceCodeCStr = klCode.c_str();

  RegisterKLExtension(client, ("SceneHub_" + prefix).toUtf8().constData(), "1.0.0", "", 1, &sourceFile, true, false);
  return prefix;
}

SHGLScene::SHGLScene(Client client, QString klFile) : m_client(client) {
  try 
  {
    QString sceneName = "SceneHub";
    if(klFile.length() > 0) sceneName = loadScene(m_client, klFile);

    m_shGLSceneVal = RTVal::Create(client, sceneName.toUtf8().constData(), 0, 0);
    m_shGLSceneVal.callMethod("", "initializeSceneAndRTR", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::SHGLScene: exception: %s\n", e.getDesc_cstr());
  }
}

FabricCore::Client SHGLScene::getClient() { 
  return m_client; 
}

FabricCore::RTVal SHGLScene::getSHGLScene() { 
  return m_shGLSceneVal; 
}

void SHGLScene::setSHGLScene(FabricCore::RTVal shGLSceneVal) { 
  m_shGLSceneVal = shGLSceneVal; 
}

void SHGLScene::setSHGLScene(SHGLScene *shGLSceneIn) { 
  m_shGLSceneVal = shGLSceneIn->getSHGLScene(); 
}

void SHGLScene::setSHGLScene(DFGBinding &binding, QString sceneName) { 
  setSHGLScene(binding.getExec().getVarValue(sceneName.toUtf8().constData())); 
}

bool SHGLScene::hasSG() {
  return m_shGLSceneVal.isValid() && getSG().isValid();
}

RTVal SHGLScene::getSG() {
  RTVal sceneVal;
  try 
  {
    sceneVal = m_shGLSceneVal.callMethod("SceneGraph", "getSG", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSG: exception: %s\n", e.getDesc_cstr());
  }
  return sceneVal;
}

RTVal SHGLScene::getSceneRoot() {
  RTVal sceneRootVal;
  try 
  {
    sceneRootVal = m_shGLSceneVal.callMethod("SGObject", "getSceneRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSceneRoot: exception: %s\n", e.getDesc_cstr());
  }
  return sceneRootVal;
}

RTVal SHGLScene::getMaterialLibraryRoot() {
  RTVal libRootVal;
  try 
  {
    libRootVal = m_shGLSceneVal.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getMaterialLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  return libRootVal;
}

RTVal SHGLScene::getImageLibraryRoot() {
  RTVal libRootVal;
  try 
  {
    libRootVal = m_shGLSceneVal.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getImageLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  return libRootVal;
}

RTVal SHGLScene::getAssetLibraryRoot() {
  RTVal libRootVal;
  try 
  {
    libRootVal = m_shGLSceneVal.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getAssetLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  return libRootVal;
}

void SHGLScene::getSceneStats(uint32_t &mesh, uint32_t &triangle, uint32_t &light) {
  try 
  {
    RTVal args[3] = {
      RTVal::ConstructUInt32(getClient(), 0),
      RTVal::ConstructUInt32(getClient(), 0),
      RTVal::ConstructUInt32(getClient(), 0)
    };
    m_shGLSceneVal.callMethod("", "getSceneStats", 3, &args[0]);
    mesh = args[0].getUInt32();
    triangle = args[1].getUInt32();
    light = args[2].getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSceneStats: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::prepareSceneForRender() {
  try 
  {
    m_shGLSceneVal.callMethod("", "prepareSceneForRender", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::prepareSceneForRender: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLScene::playbackByDefault() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "playbackByDefault", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::playbackByDefault: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

void SHGLScene::getInitialTimelineState(bool &enable, int &start, int &end, float &fps) {
  try 
  {
    RTVal args[4] = {
      RTVal::ConstructBoolean(getClient(), 0),
      RTVal::ConstructSInt32(getClient(), 0),
      RTVal::ConstructSInt32(getClient(), 0),
      RTVal::ConstructFloat32(getClient(), 0)
    };
    m_shGLSceneVal.callMethod("", "getInitialTimelineState", 4, &args[0]);
    enable = args[0].getBoolean();
    start = args[1].getSInt32();
    end = args[2].getSInt32();
    fps = args[3].getFloat32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getInitialTimelineState: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::setFrame(int frame) {
  try 
  {
    RTVal arg = RTVal::ConstructSInt32(getClient(), frame);    
    m_shGLSceneVal.callMethod("", "setFrame", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setOrthographicViewport: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLScene::sceneHierarchyChanged() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::setOrthographicViewport: sceneHierarchyChanged: %s\n", e.getDesc_cstr());
  }
  return false;
}

RTVal SHGLScene::getSelectedObjects() {
  RTVal selectedObject;
  try 
  {
    selectedObject = m_shGLSceneVal.callMethod("SGObject[]", "getSelectedObjects", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setPlayback: exception: %s\n", e.getDesc_cstr());
  }
  return selectedObject;
}

void SHGLScene::sceneItemSelected(RTVal obj) {
  try 
  {
    m_shGLSceneVal.callMethod("", "sceneItemSelected", 1, &obj);
  }
  catch(Exception e)
  {
    printf("SHGLScene::sceneItemSelected: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::treeItemSelected(SHTreeItem *item) {
  treeItemSelected(item->getSGObject());
}

void SHGLScene::treeItemSelected(RTVal obj) {
  try 
  {
    m_shGLSceneVal.callMethod("", "treeItemSelected", 1, &obj);
  }
  catch(Exception e)
  {
    printf("SHGLScene::treeItemSelected: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::treeItemDeselected(SHTreeItem *item) {
  treeItemDeselected(item->getSGObject());
}

void SHGLScene::treeItemDeselected(RTVal obj) {
  try 
  {
    m_shGLSceneVal.callMethod("", "treeItemDeselected", 1, &obj);
  }
  catch(Exception e)
  {
    printf("SHGLScene::treeItemDeselected: exception: %s\n", e.getDesc_cstr());
  }
}

QString SHGLScene::getTreeItemPath(SHTreeItem *item) {
  QString url = "none";
  try 
  {
    RTVal sgObject = item->getSGObject();
    RTVal sgParent = sgObject.callMethod("SGObject", "getOwnerInstance", 0, 0);

    if(sgParent.callMethod("Boolean", "isValid", 0, 0).getBoolean())
    {
      RTVal parentNameVal = sgParent.callMethod("String", "getName", 0, 0);
      RTVal typeVal = sgObject.callMethod("String", "type", 0, 0);
      QString parentName = QString(parentNameVal.getStringCString());
      QString type = QString(typeVal.getStringCString());
      
      // Assets
      if(parentName == "assets" || parentName == "images")
      {
        RTVal param = RTVal::ConstructString(m_client, "path");
        RTVal sgProperty = sgObject.callMethod("SGObjectProperty", "getLocalProperty", 1, &param);
        if(sgProperty.callMethod("Boolean", "isValid", 0, 0).getBoolean())
        {
          sgProperty.callMethod("", "getValue", 1, &param);
          // Create data
          url = QString(QString("file://") + QString(param.getStringCString()));
        }
      }
    }
  }
  catch(Exception e)
  {
    printf("SHGLScene::getTreeItemPath: exception: %s\n", e.getDesc_cstr());
  }

  return url;
}

QStringList SHGLScene::getSceneNamesFromBinding(DFGBinding &binding) {

  FabricCore::DFGStringResult json =  binding.getVars();
  FTL::JSONStrWithLoc jsonStrWithLoc( json.getCString() );
  FTL::OwnedPtr<FTL::JSONObject> jsonObject(FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>() );

  QList<FTL::JSONObject const *> objects;
  objects.append(jsonObject.get());
  QStringList sceneNameList;

  for(int i=0; i<objects.size(); i++)
  {
    FTL::JSONObject const * varsObject = objects[i]->maybeGetObject( FTL_STR("vars") );
    if(varsObject)
    {
      for(FTL::JSONObject::const_iterator it = varsObject->begin(); it != varsObject->end(); it++)
      {
        QString sceneName(it->first.c_str());
        FTL::JSONObject const *value = it->second->cast<FTL::JSONObject>();
        for(FTL::JSONObject::const_iterator jt = value->begin(); jt != value->end(); jt++) 
        {
          if(QString(jt->second->getStringValue().c_str()) == "SHGLScene")
          {
            if(!sceneNameList.contains(sceneName))
              sceneNameList.append(sceneName);
          }
        }
      }
    }
  }
  return sceneNameList;
}

bool SHGLScene::selectionChangedFromManips() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "selectionChangedFromManips", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::selectionChangedFromManips: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

QString SHGLScene::getSelectionCategory() {
  try 
  {
    return QString(m_shGLSceneVal.callMethod("String", "getSelectionCategory", 0, 0).getStringCString());
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return QString();
}

bool SHGLScene::showTreeViewByDefault(uint32_t &level) {
  bool show = false;
  try 
  {
    RTVal levelVal = RTVal::ConstructUInt32(getClient(), level); 
    show = m_shGLSceneVal.callMethod("Boolean", "showTreeViewByDefault", 1, &levelVal).getBoolean();
    level = levelVal.getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return show;
}

void SHGLScene::addExternalFileList(QStringList pathList, bool expand, float x, float y, float z) {
  try 
  {
    RTVal klPathList = RTVal::ConstructVariableArray(getClient(), "String");
    klPathList.setArraySize(pathList.size());
    for(int i=0; i<pathList.size(); ++i) 
      klPathList.setArrayElement(i, RTVal::ConstructString(getClient(), pathList[i].toUtf8().constData()));

    RTVal posVal = RTVal::Construct(getClient(), "Vec3", 0, 0);
    posVal.setMember("x", RTVal::ConstructFloat32(getClient(), x));
    posVal.setMember("y", RTVal::ConstructFloat32(getClient(), y));
    posVal.setMember("z", RTVal::ConstructFloat32(getClient(), z));

    RTVal args[3] = {
      klPathList,
      posVal,
      RTVal::ConstructBoolean(getClient(), expand)
    };
    m_shGLSceneVal.callMethod("", "addExternalFileList", 3, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::addExternalFileList: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::setObjectColor(QColor color, bool local) {
  try 
  {
    RTVal colorVal = RTVal::Construct(getClient(), "Color", 0, 0);
    colorVal.setMember("r", RTVal::ConstructFloat32(getClient(), color.redF()));
    colorVal.setMember("g", RTVal::ConstructFloat32(getClient(), color.greenF()));
    colorVal.setMember("b", RTVal::ConstructFloat32(getClient(), color.blueF()));
    colorVal.setMember("a", RTVal::ConstructFloat32(getClient(), color.alpha()));

    RTVal args[2] = {
      colorVal,
      RTVal::ConstructBoolean(getClient(), local)
    };
    m_shGLSceneVal.callMethod("", "setObjectColor", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setObjectColor: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::addLight(uint32_t lightType, float x, float y, float z) {
  try 
  {
    RTVal posVal = RTVal::Construct(getClient(), "Vec3", 0, 0);
    posVal.setMember("x", RTVal::ConstructFloat32(getClient(), x));
    posVal.setMember("y", RTVal::ConstructFloat32(getClient(), y));
    posVal.setMember("z", RTVal::ConstructFloat32(getClient(), z));

    RTVal args[2] = {
      RTVal::ConstructUInt32(getClient(), lightType),
      posVal
    };
    m_shGLSceneVal.callMethod("", "addLight", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::addLight: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::setlightProperties(QColor color, float intensity) {
  try 
  {
    RTVal colorVal = RTVal::Construct(getClient(), "Color", 0, 0);
    colorVal.setMember("r", RTVal::ConstructFloat32(getClient(), color.redF()));
    colorVal.setMember("g", RTVal::ConstructFloat32(getClient(), color.greenF()));
    colorVal.setMember("b", RTVal::ConstructFloat32(getClient(), color.blueF()));
    colorVal.setMember("a", RTVal::ConstructFloat32(getClient(), color.alpha()));

    RTVal args[2] = {
      colorVal,
      RTVal::ConstructFloat32(getClient(), intensity)
    };
    m_shGLSceneVal.callMethod("", "setlightProperties", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setlightProperties: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::exportToAlembic(QString filePath) {
  try 
  {
    RTVal filePathVal = RTVal::ConstructString(getClient(), filePath.toUtf8().constData());
    m_shGLSceneVal.callMethod("", "exportToAlembic", 1, &filePathVal);
  }
  catch(Exception e)
  {
    printf("SHGLScene::exportToAlembic: exception: %s\n", e.getDesc_cstr());
  }
}


// ****************
RTVal SHGLScene::getCmdManager() {
  RTVal cmdManager;
  try 
  {
    cmdManager = m_shGLSceneVal.callMethod("CmdManager", "getOrCreateCmdManager", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getCmdManager: exception: %s\n", e.getDesc_cstr());
  }
  return cmdManager;
}

RTVal SHGLScene::setParamValue(std::string type, std::string value) {
  RTVal cmdManager, paramsVal;
  try 
  {
    paramsVal = RTVal::Construct(getClient(), type.c_str(), 0, 0);
    DecodeRTValFromJSON(getClient(), paramsVal, value.c_str());
  }
  catch(Exception e)
  {
    printf("SHGLScene::setParamValue: exception: %s\n", e.getDesc_cstr());
  }
  return paramsVal;
}

RTVal SHGLScene::retrieveCmd(uint32_t index) {
  RTVal cmdVal;
  try 
  {
    RTVal indexVal = RTVal::ConstructUInt32(getClient(), index);
    cmdVal = getCmdManager().callMethod("SGBaseCmd", "getCmdInUndoStack", 1, &indexVal);
  }
  catch(Exception e)
  {
    printf("SHGLScene::retrieveCmd: exception: %s\n", e.getDesc_cstr());
  }
  return cmdVal;
}

uint32_t SHGLScene::getNumCmdInUndoStack() {
  try 
  {
    return getCmdManager().callMethod("Size", "getNumCmdInUndoStack", 0, 0).getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getNumCmdInUndoStack: exception: %s\n", e.getDesc_cstr());
  }
  return 0;
}

void SHGLScene::executeCmd(std::string cmdName, std::vector<RTVal> &params)  {
  try 
  {
    getSG().callMethod("", cmdName.c_str(), params.size(), &params[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::executeCmd: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::undoCmd(uint32_t undoCount) {
  try 
  {
    for(unsigned i=0; i<undoCount; ++i)
      getCmdManager().callMethod("Boolean", "undo", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::undoCmd: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::redoCmd(uint32_t redoCount) {
  try 
  {
    for(unsigned i=0; i<redoCount; ++i)
      getCmdManager().callMethod("Boolean", "redo", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::redoCmd: exception: %s\n", e.getDesc_cstr());
  }
}


// ****************
std::string SHGLScene::EncodeRTValToJSON(Client client, RTVal rtVal) {
  if(rtVal.isValid())
  {
    if(rtVal.isObject())
    {
      if(!rtVal.isNullObject())
      {
        RTVal cast = RTVal::Construct(client, "RTValToJSONEncoder", 1, &rtVal);
        if(cast.isValid())
        {
          if(!cast.isNullObject())
          {
            RTVal result = cast.callMethod("String", "convertToString", 0, 0);
            if(result.isValid())
            {
              FTL::CStrRef ref = result.getStringCString();
              if(ref.size() > 0)
              {
                std::string json;
                {
                  FTL::JSONEnc<> jsonEnc( json );
                  FTL::JSONStringEnc<> jsonStringEnc( jsonEnc, ref );
                }
                return json;
              }
            }
          }
        }
      }
    }
  }
  RTVal valueJSON = rtVal.getJSON();
  return valueJSON.getStringCString();
}

void SHGLScene::DecodeRTValFromJSON(Client client, RTVal &rtVal, FTL::CStrRef json) {

  if(json.size() > 2)
  {
    try
    {
      if(rtVal.isValid())
      {
        if(rtVal.isObject())
        {
          if(rtVal.isNullObject())
            rtVal = RTVal::Create( client, rtVal.getTypeName().getStringCString(), 0, NULL );
          
          std::string decodedString;
          {
            FTL::JSONStrWithLoc strWithLoc( json );
            FTL::JSONDec jsonDec( strWithLoc );
            FTL::JSONEnt jsonEnt;
            if ( jsonDec.getNext( jsonEnt ) && jsonEnt.getType() == jsonEnt.Type_String )
              jsonEnt.stringAppendTo( decodedString );
          }

          if(decodedString.length() > 0)
          {
            RTVal cast = RTVal::Construct(client, "RTValFromJSONDecoder", 1, &rtVal);
            if(cast.isInterface())
            {
              if(!cast.isNullObject())
              {
                RTVal data =
                  RTVal::ConstructString(
                    client,
                    decodedString.data(),
                    decodedString.size()
                    );
                RTVal result = cast.callMethod("Boolean", "convertFromString", 1, &data);
                if(result.isValid())
                {
                  if(result.getBoolean())
                  {
                    return;
                  }
                }
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {
      printf("decodeRTValFromJSON: Hit exception: %s\n", e.getDesc_cstr());
    }
  }

  rtVal.setJSON( json.c_str() );
}
