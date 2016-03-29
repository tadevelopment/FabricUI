/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include "SHEditorWidget.h"
#include "SHLightEditorDialog.h"
#include "SHGeometryEditorDialog.h"
#include <FabricUI/Viewports/QtToKLEvent.h>
#include <FabricUI/Util/StringUtils.h>

using namespace FabricCore;
using namespace FabricUI::Util;
using namespace FabricUI::SceneHub;


// *************************
void SHEditorWidget::AddExternalFileList(SHGLScene *shGLScene, QStringList pathList, float *pos, bool expand) {
  QStringList pathList_ = StringUtils::ProcessPathQStringForOsX(pathList);
  if(pathList_.size() > 0)
  {
    try 
    {
      shGLScene->addExternalFileList(pathList_, expand, pos[0], pos[1], pos[2]);
    }
    catch(Exception e)
    {
      printf("SHEditorWidget::AddExternalFileList: exception: %s\n", e.getDesc_cstr() );
    }
  }
}

void SHEditorWidget::AddExternalFile(SHGLScene *shGLScene, QString path, float *pos, bool expand) { 
  QStringList pathList; pathList.append(path);
  AddExternalFileList(shGLScene, pathList, pos, expand);
}

void SHEditorWidget::ImportAsset(SHGLScene *shGLScene) { 
  QFileDialog dialog(0, "Import Assets", "", "*.abc *.fbx");
  dialog.setFileMode(QFileDialog::ExistingFiles);
  if(dialog.exec())
  {
    QStringList pathList = dialog.selectedFiles();
    if(pathList.size() > 0)
    {
      float pos[3] = { 0.0f, 0.0f, 0.0f };
      AddExternalFileList(shGLScene, pathList, pos, false);
    }
  }
}

void SHEditorWidget::ExportToAlembic(SHGLScene *shGLScene) { 

  QString filePath = QFileDialog::getSaveFileName(0, "Export scene to Alembic", "", "*.abc");
  if(filePath.length() == 0)
    return;
  if(filePath.toLower().endsWith(".abs.abc"))
    filePath = filePath.left(filePath.length() - 4);
  else if(!filePath.toLower().endsWith(".abc"))
    filePath += ".abc";

  try 
  {
    QStringList pathList; pathList.append(filePath);
    pathList = StringUtils::ProcessPathQStringForOsX(pathList);
    shGLScene->exportToAlembic(pathList[0]);
  }
  catch(Exception e)
  {
    printf("SHEditorWidget::ExportToAlembic: exception: %s\n", e.getDesc_cstr() );
  }
}


// *************************
SHEditorWidget::SHEditorWidget(QWidget* parent, SHGLScene *shGLScene, QPoint point) 
  : QMenu(parent)
  , m_shGLScene(shGLScene)
{
  QString category = m_shGLScene->getSelectionCategory();
  if(point.x() > 0) 
  {
    //m_screenPos = point;
    if(category == "") constuctAddMenu();
    else if(category == "geometry" || category == "parent") constuctGeometryMenu(category);
    else if(category == "light") constuctLightMenu();
  }
  else
  {
    if(category == "geometry" || category == "parent") constuctGeometryMenu(category);
    else if (category == "light") editLightProperties();
  }
}

void SHEditorWidget::constuctAddMenu() {
  QAction* addArchiveAction = addAction("Add Geometry");
  QAction* addArchiveExpandAction = addAction("Add Geometry (Force Expand");
  addSeparator();
  QAction* addSpotLightAction = addAction("Add Spot");
  QAction* addPointLightAction = addAction("Add Point");
  QAction* addDirectLightAction = addAction("Add Directional");
  QAction* addShadowSpotLightAction = addAction("Add Shadow Spot");
  QAction* addShadowPointLightAction = addAction("Add Shadow Point");
  QAction* addShadowDirectLightAction = addAction("Add Shadow Directional");
 
  QObject::connect(addArchiveAction, SIGNAL(triggered()), this, SLOT(addArchive()));
  QObject::connect(addArchiveExpandAction, SIGNAL(triggered()), this, SLOT(addArchive()));
  QObject::connect(addSpotLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addPointLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addDirectLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addShadowSpotLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addShadowPointLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
  QObject::connect(addShadowDirectLightAction, SIGNAL(triggered()), this, SLOT(addLight()));
}

void SHEditorWidget::constuctGeometryMenu(QString category) {
  // Directly call the color box
  if(category == "parent")
    editObjectColor();

  // However, a geometry can either has color ortexture
  else if(category == "geometry")
  {
    QAction* editColor = addAction("Edit Color");
    QObject::connect(editColor, SIGNAL(triggered()), this, SLOT(editObjectColor()));
    QAction* editLocalColor = addAction("Edit local Color");
    QObject::connect(editLocalColor, SIGNAL(triggered()), this, SLOT(editLocalObjectColor()));
    QAction* setTextureAction = addAction("Set Texture");
    QObject::connect(setTextureAction, SIGNAL(triggered()), this, SLOT(addTexture()));
  }
}

void SHEditorWidget::constuctLightMenu() {
  setTitle("Light");
  QAction* propertiesAction = addAction("Edit Properties");
  QObject::connect(propertiesAction, SIGNAL( triggered() ),this, SLOT( editLightProperties() ) );
}

void SHEditorWidget::addLight() {
  // Get the light we want to add
  uint32_t lightType = 0;
  QString lightName = dynamic_cast<QAction*>(QObject::sender())->text();
  if(lightName == "Add Point") lightType = 0;
  else if(lightName == "Add Spot") lightType = 1;
  else if(lightName == "Add Directional") lightType = 2;
  else if(lightName == "Add Shadow Point") lightType = 3;
  else if(lightName == "Add Shadow Spot") lightType = 4;
  else if(lightName == "Add Shadow Directional") lightType = 5;
  m_shGLScene->addLight(lightType, 0.0f, 0.0f, 0.0f);
}

void SHEditorWidget::addArchive() {
  bool expand = false;
  QString senderName = dynamic_cast<QAction*>( QObject::sender())->text();
  if(senderName == "Add Geometry") expand = true;
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilter(tr("Files (*.abc *.fbx)"));
  QStringList pathList;
  if(dialog.exec())
  {
    pathList = dialog.selectedFiles();
    float pos[3] = { 0.0f, 0.0f, 0.0f };
    AddExternalFileList(m_shGLScene, pathList, pos, expand);
  }
}

void SHEditorWidget::addTexture() {
  bool expand = false;
  //std::string name = dynamic_cast<QAction*>( QObject::sender())->text().toStdString();
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  QStringList pathList;
  if(dialog.exec())
  {
    pathList = dialog.selectedFiles();
    float pos[3] = { 0.0f, 0.0f, 0.0f };
    AddExternalFileList(m_shGLScene, pathList, pos, expand);
  }
}

void SHEditorWidget::editObjectColor( bool local ) {
  QColor color = QColorDialog::getColor(Qt::yellow, this);
  if(color != QColor::Invalid) m_shGLScene->setObjectColor(color, local);
}

void SHEditorWidget::editLightProperties() {
  //if(m_geometryDialog->isVisible())
  //  m_geometryDialog->close();
  //int w = int(0.2*this->width());
  //w = (w < 50) ? 50 : w;
  //int h = int(0.2*this->height());
  //h = (h < 50) ? 50 : h;
  //SHLightEditorDialog *lightDialog = new SHLightEditorDialog(
  //  this, 
  //  m_shGLScene);
  //lightDialog->setFixedSize(w, h);
  //lightDialog->show();
}
