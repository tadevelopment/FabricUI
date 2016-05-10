/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SCENE_H__
#define __UI_SCENEHUB_SCENE_H__

#include <QtGui/QDrag>
#include <QtGui/QColor>
#include <QtGui/QVector3D>
#include <QtGui/QMouseEvent>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <FabricCore.h>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>
#include <FabricUI/DFG/DFGController.h>
 
namespace FabricUI {
namespace SceneHub {
    
class SHGLScene {

  /**
    SHGLScene is a helper class wrapping :kl-ref:`SHGLScene`.
    It allows easy and safe access in C++ to the KL methods.

    When possible, the logic is to write most of the code in :kl-ref:`SHGLScene`.
  */


  public:
    SHGLScene(FabricCore::Client client) : m_client(client) {}

    SHGLScene(FabricCore::Client client, QString klFile);
    
    /// Gets the client.
    FabricCore::Client getClient();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getSHGLScene();

    /// Gets a reference to the renderer.
    FabricCore::RTVal getSHGLRenderer();

    /// Sets a reference to the scenegraph.
    void setSHGLScene(FabricCore::RTVal shGLSceneVal);

    /// Sets a reference to the scenegraph.
    void setSHGLScene(SHGLScene *shGLSceneIn);

    void setSHGLScene(FabricCore::DFGBinding &binding, QString sceneName);

    /// Checks a scenegraph is set.
    bool hasSG();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getSG();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getSceneRoot();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getMaterialLibraryRoot();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getImageLibraryRoot();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getAssetLibraryRoot();

    /// Gets the drawing statistics.
    QList<unsigned int> getSceneStats();
    
    /// Prepares for rendering. 
    /// This must be called once before re-drawing all viewports.
    void prepareSceneForRender();

    /// Gets the initiale time-line state, for playback.
    /// \param enable Is the playback active.
    /// \param startFrame The timeline first frame.
    /// \param endFrame The timeline last frame.
    /// \param fps The number of frames per second.
    void getInitialTimelineState(bool &enable, int &startFrame, int &endFrame, float &fps);

    /// Enables the time line when the app opens.
    bool enableTimelineByDefault();

    /// Gets the frame per second
    float getFPS();

    /// Gets the start-end frames
    QList<int> getFrameState();

    /// Activates the playback at app opening.
    bool playbackByDefault();

    /// Refresh the viewport constantly as fast as possible
    bool refreshAlways();

    /// Gets the path of the current selected object.
    QString getTreeItemPath(FabricCore::RTVal sgObject);

    /// Gets the path of the current selected object.
    QStringList getSceneNamesFromBinding(FabricCore::DFGBinding &binding);

    /// Adds texture or asset files.
    /// Contains a list of file paths associated with a mouse position. 
    /// Depending on their type, a Scene hierarchy or a texture might be created.
    /// \param pathList The array of pathes to read
    /// \param pos The scene position, use to asset/texture placement
    /// \param expand Force expension
    void addExternalFileList(QStringList pathList, bool expand, float x=0, float y=0, float z=0);

    /// Updates selected intance(s) with provided properties
    /// \param color The instance color
    /// \param local Local proprety if true
    void setObjectColor(QColor color, bool local);

    /// Adding lights
    /// \param lightType The type of light (spot, point, diectional...)
    void addLight(unsigned int lightType, float x=0, float y=0, float z=0);

    /// Updates selected light(s) with provided properties
    /// \param color The light color
    /// \param intensisty The light intensisty
    void setlightProperties(QColor color, float intensity);

    /// Exports the scene to alembic.
    /// \param filePath The path of the alembic file.
    void exportToAlembic(QString filePath);

    /// Exports a scene object to alembic.
    /// \param filePath The path of the alembic file.
    void exportToAlembic( FabricCore::RTVal sgObject, QString filePath );

    /// Shows the treeView when the app opens.
    /// \param level Iniitial level of expension
    QPair<bool, unsigned int> showTreeViewByDefault();

    /// If true, show the value editor when initializing, and set its target to defaultTarget
    bool showValueEditorByDefault();

    /// If valid, this RTVal contains the SGCanvasOperator to show by default in the DFG
    FabricCore::RTVal getDefaultSGCanvasOperator();

    /// Gets the value-editor default target for this scene.
    FabricCore::RTVal getValueEditorDefaultTarget();
  
 
  private:
    /// \internal
    FabricCore::Client m_client;    
    /// \internal
    FabricCore::RTVal m_shGLSceneVal;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_SCENE_H__
