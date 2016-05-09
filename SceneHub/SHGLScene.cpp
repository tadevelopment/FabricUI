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

FabricCore::RTVal SHGLScene::getSHGLRenderer() {
  FabricCore::RTVal val;
  try {
    val = m_shGLSceneVal.callMethod( "SHGLRenderer", "getSHRenderer", 0, 0 );
  }
  catch( Exception e ) {
    printf( "SHGLScene::getSHGLRenderer: exception: %s\n", e.getDesc_cstr() );
  }
  return val;
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

QList<unsigned int> SHGLScene::getSceneStats() {
  QList<unsigned int> stats;
  try 
  {
    RTVal args[3] = {
      RTVal::ConstructUInt32(getClient(), 0),
      RTVal::ConstructUInt32(getClient(), 0),
      RTVal::ConstructUInt32(getClient(), 0)
    };
    m_shGLSceneVal.callMethod("", "getSceneStats", 3, &args[0]);
    // Mesh
    stats.append(args[0].getUInt32());
    // Triangles
    stats.append(args[1].getUInt32());
    // Lights
    stats.append(args[2].getUInt32());
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSceneStats: exception: %s\n", e.getDesc_cstr());
  }
  return stats;
}

void SHGLScene::prepareSceneForRender() {
  
  try 
  {
    m_shGLSceneVal.callMethod("", "prepareSceneForRender", 0, NULL);
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

bool SHGLScene::refreshAlways() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "refreshAlways", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::refreshAlways: exception: %s\n", e.getDesc_cstr());
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

bool SHGLScene::enableTimelineByDefault() {
  int start, end; 
  float fps; bool enable;
  getInitialTimelineState(enable, start, end, fps);
  return enable;
}

float SHGLScene::getFPS() {
  int start, end; 
  float fps; bool enable;
  getInitialTimelineState(enable, start, end, fps);
  return fps;
}

QList<int> SHGLScene::getFrameState() {
  int start, end; 
  float fps; bool enable;
  getInitialTimelineState(enable, start, end, fps);
  QList<int> state; state.append(start); state.append(end);
  return state;
}

QString SHGLScene::getTreeItemPath(RTVal sgObject) {
  QString url;
  if(!sgObject.isValid()) return url;
  try 
  {
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

QPair<bool, unsigned int> SHGLScene::showTreeViewByDefault() {
  QPair<bool, unsigned int> pair;
  // Show the treeView
  pair.first = false;
  pair.second = 1;
  try 
  {
    RTVal levelVal = RTVal::ConstructUInt32(getClient(), pair.second); 
    pair.first = m_shGLSceneVal.callMethod("Boolean", "showTreeViewByDefault", 1, &levelVal).getBoolean();
    pair.second = levelVal.getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::showTreeViewByDefault: exception: %s\n", e.getDesc_cstr());
  }
  return pair;
}

bool SHGLScene::showValueEditorByDefault() {
  bool show = false;
  try {
    FabricCore::RTVal defaultTarget = FabricCore::RTVal::Construct( m_client, "SGObject", 0, 0 );
    show = m_shGLSceneVal.callMethod( "Boolean", "showValueEditorByDefault", 1, &defaultTarget ).getBoolean();
  }
  catch( Exception e ) {
    printf( "SHGLScene::showValueEditorByDefault: exception: %s\n", e.getDesc_cstr() );
  }
  return show;
}

FabricCore::RTVal SHGLScene::getDefaultSGCanvasOperator() {
  FabricCore::RTVal result;
  try {
    result = m_shGLSceneVal.callMethod( "SGCanvasOperator", "getDefaultSGCanvasOperator", 0, 0 );
    if( result.isNullObject() )
      result = FabricCore::RTVal();
  }
  catch( Exception e ) {
    printf( "SHGLScene::getDefaultSGCanvasOperator: exception: %s\n", e.getDesc_cstr() );
  }
  return result;
}

FabricCore::RTVal SHGLScene::getValueEditorDefaultTarget() {
  FabricCore::RTVal defaultTarget;
  try {
    defaultTarget = FabricCore::RTVal::Construct( m_client, "SGObject", 0, 0 );
    m_shGLSceneVal.callMethod( "Boolean", "showValueEditorByDefault", 1, &defaultTarget );
  }
  catch( Exception e ) {
    printf( "SHGLScene::showValueEditorByDefault: exception: %s\n", e.getDesc_cstr() );
  }
  return defaultTarget;
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

void SHGLScene::addLight(unsigned int lightType, float x, float y, float z) {
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

void SHGLScene::exportToAlembic( RTVal sgObject, QString filePath ) {
  try {
    RTVal args[2];
    args[0] = sgObject;
    args[1] = RTVal::ConstructString( getClient(), filePath.toUtf8().constData() );
    m_shGLSceneVal.callMethod( "", "exportToAlembic", 2, args );
  }
  catch( Exception e ) {
    printf( "SHGLScene::exportToAlembic: exception: %s\n", e.getDesc_cstr() );
  }
}
