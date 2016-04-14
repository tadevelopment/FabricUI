/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SCENE_H__
#define __UI_SCENEHUB_SCENE_H__

#include <QtGui/QDrag>
#include <QtGui/QColor>
#include <QtGui/QVector3D>
#include <QtGui/QMouseEvent>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <FabricCore.h>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/SceneHub/TreeView/SHTreeItem.h>

namespace FabricUI {
namespace SceneHub {
    
class SHGLScene {

  public:
    SHGLScene(FabricCore::Client client) : m_client(client) {}

    SHGLScene(FabricCore::Client client, QString klFile);

    ~SHGLScene() {}
    
    /// Gets the client.
    FabricCore::Client getClient();

    /// Gets a reference to the scenegraph.
    FabricCore::RTVal getSHGLScene();

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
    void getSceneStats(unsigned int &meshCount, unsigned int &triangleCount, unsigned int &lightCount);
    
    /// Prepares for rendering. 
    /// This must be called once before re-drawing all viewports.
    void prepareSceneForRender();

    /// Gets the initiale   time-line state, for playback.
    /// \param enable Is the playback active.
    /// \param startFrame The timeline first frame.
    /// \param endFrame The timeline last frame.
    /// \param fps The number of frames per second.
    void getInitialTimelineState(bool &enable, int &startFrame, int &endFrame, float &fps);

    /// Activates the playback at app opening.
    bool playbackByDefault();
    
    /// Sets the current frame, for animation.
    /// \param frame The frame number
    void setFrame(int frame);

    /// Returns true if the structure of the SceneGraph changed since the last call to this function.
    /// This allows the C++ app to filter multiple consecutive "sceneChanged" notifications.
    bool sceneHierarchyChanged(); 

    /// Returns an array of SGObject that are currently in the 'selection' set. 
    /// This is used for reflecting the 3D selection to the TreeView (which might or not have these as UI nodes).
    FabricCore::RTVal getSelectedObjects(); 

    /// Updates the tree-view when selection from the scene.
    /// \param sgObject The selected scenegraph object
    void sceneItemSelected(FabricCore::RTVal sgObject); 

    /// Updates the tree-view when selection from the scene.
    /// \param item The selected SHTreeItem
    void treeItemSelected(SHTreeItem *item); 

    /// Updates the 'selection' from TreeView's selection.
    /// \param sgObject The selected scenegraph object
    void treeItemSelected(FabricCore::RTVal sgObject); 

    /// Updates the 'selection' from TreeView's selection.
    /// \param item The deselected SHTreeItem
    void treeItemDeselected(SHTreeItem *item); 
    
    /// Updates the 'selection' from TreeView's selection.
    /// \param sgObject The deselected scenegraph object
    void treeItemDeselected(FabricCore::RTVal sgObject); 
                
    /// Gets the path of the current selected object.
    QString getTreeItemPath(SHTreeItem *item);

    /// Gets the path of the current selected object.
    QString getTreeItemPath(FabricCore::RTVal sgObject);

    /// Gets the path of the current selected object.
    QStringList getSceneNamesFromBinding(FabricCore::DFGBinding &binding);

    /// Checks if the selection changed from the manipulation system.
    /// Synchronizes with the tree-view.
    bool selectionChangedFromManips();
    
    /// Gets the category of the selection.
    /// Used to know what type of element is selected.
    /// For showing the right contextual menu.
    QString getSelectionCategory();

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

    /// Shows the treeView when the app opens.
    /// \param level Iniitial level of expension
    bool showTreeViewByDefault(unsigned int &level);

    /// If true, show the value editor when initializing, and set its target to defaultTarget
    bool showValueEditorByDefault( FabricCore::RTVal &defaultTarget );

    /// Gets the command manager.
    FabricCore::RTVal getCmdManager();

    /// From the parameter type and its value, creates 
    /// \param type The parameter type
    /// \param value The parameter value JSon encoded
    FabricCore::RTVal setParamValue(QString type, QString value);
    
    /// Gets the command at index i of KL stack.
    /// \param index The indo of the commands in the stack.
    FabricCore::RTVal retrieveCmd(unsigned int index);

    /// Gets the number of commands in the kl stacks.
    /// Uses for synchronizaztion
    unsigned int getNumCmdInUndoStack();

    /// Executes the command at index i of KL stack.
    /// \param cmdName The name of the commands
    /// \param params The paramters
    void executeCmd(QString cmdName, QList<FabricCore::RTVal> &params);

    /// Undo commands.
    /// \param undoCount The number of undos. 
    void undoCmd(unsigned int undoCount);

    /// Redo commands.
    /// \param redoCount The number of redos. 
    void redoCmd(unsigned int redoCount);
 

  private:
    /// Encodes a rtVal into a Json, saves the rtVal
    /// \param rtVal The value to encode
    std::string EncodeRTValToJSON(FabricCore::Client client, FabricCore::RTVal rtVal);

    /// Decodes a rtVal from a Json, reads the rtVal
    /// \param client The core client
    /// \param rtVal The result value
    /// \param json The string to decode
    void DecodeRTValFromJSON(FabricCore::Client client, FabricCore::RTVal &rtVal, FTL::CStrRef json); 
    
    /// \internal
    FabricCore::Client m_client;    
    /// \internal
    FabricCore::RTVal m_shGLSceneVal;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_SCENE_H__
