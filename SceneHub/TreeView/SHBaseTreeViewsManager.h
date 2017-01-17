/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_BASE_TREE_VIEW_MANAGER_H__
#define __UI_SCENEHUB_BASE_TREE_VIEW_MANAGER_H__

#include "SHBaseTreeView.h"
#include <QWidget>
#include <QComboBox>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/SceneHub/SHStates.h>
#include <FabricUI/SceneHub/SHGLScene.h>
 
namespace FabricUI {
namespace SceneHub {

class SHTreeComboBox : public QComboBox {

  /**
    SHTreeComboBox specializes QtGui::QComboBox.
    When the combobox is manipulated, a signal is emitted before it's displayed.
    
    It's used to look for SHGLScene that can be defined in the canvas graph
    and update the combobox items list.
  */

  Q_OBJECT 
  
  public:
    /// Constructor.
    /// \param parent A reference to the QObject parent (can be null).
    SHTreeComboBox(QWidget *parent = 0) 
      : QComboBox(parent) {
    }
    
    /// Implementation of QtGui.QComboBox
    virtual void showPopup() {
      emit updateSceneList();
      QComboBox::showPopup();
    }

  signals:
    /// Emitted when the combobox content is displayed.
    void updateSceneList();
};


class SHBaseTreeViewsManager : public QWidget { 

  /**
    SHBaseTreeViewsManager owns a SHTreeView and allows to display 
    the ojects/properties of a SHGLScene.

    It defines a base class for the SHTreeViewsManager defined in C++ and python.
    
    The SHGLScenes are defined within the canvas graph as canvas nodes.
    A SHTreeComboBox is used to display all the available scenes in the app and switch among them.
    The treeView is then updated to display the objects of the selected scene.

    The class gives also access to the base signals/slots so it can be specialized if needed.
  */

  Q_OBJECT

  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param dfgWidget A reference to the DFG::DFGWidget.
    /// \param shStates A reference to the SceneHub::SHStates.
    SHBaseTreeViewsManager(
      FabricCore::Client client,
      DFG::DFGWidget *dfgWidget, 
      SHStates *shStates);

    virtual ~SHBaseTreeViewsManager();
    
    /// Gets the client.
    FabricCore::Client getClient();
    
    /// Gets a pointer to the treeView.
    SHBaseTreeView* getTreeView();
    
    /// Gets a reference to the current SHGLScene.
    SHGLScene* getScene();

    /// Shows the SGObjectProperties in the treeView.
    void setShowProperties(bool show);
    
    /// Shows the canvas operators in the treeView.
    void setShowOperators(bool show);

    /// Expands the treeView at level 'level'.
    void expandTree(unsigned int level);


  public slots:
    /// Called when the selection changed.
    /// Called if changes from SHStates.
    void onSelectionChanged();

    /// Updates the scene (if structural changes).
    /// Called if changes from the treeView or SHStates.
    void onSceneHierarchyChanged();

    /// Updates the scene (if treeView selection changed).
    /// Called if changes from the treeView or SHStates.
    void onSceneChanged();
    

  signals:
    /// Emitted if the the scene changed (selection).
    void sceneChanged();

    /// Emitted if the the scene changed (if structural changes).
    void sceneHierarchyChanged();

    /// Emitted if the the scene changed. 
    void activeSceneChanged(FabricUI::SceneHub::SHGLScene *scene);


  public slots:
    /// Constructs the TreeView from the selected scene,
    /// the main scene, or from a scene defined in a DFG node.
    virtual void onConstructScene(const QString &sceneName);

    /// Finds all the SHGLScenes (in the app and in the canvas graph)
    /// and populates the combobox with their name.
    virtual void onUpdateSceneList();

    /// Called when the selection is cleared.
    /// Updates the SHStates.
    void onSelectionCleared();

    /// Updates the current selection.
    /// Updates the SHStates.
    void onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *item);
    
    /// Updates the current selection.
    /// Updates the SHStates.
    void onTreeItemDeselected(FabricUI::SceneHub::SHTreeItem *item);
    
    /// Updates the selection properties in the valueEditor.
    /// Updates the SHStates.
    void onTreeItemDoubleClicked(FabricUI::SceneHub::SHTreeItem *item);


  protected:
    /// Resets the TreeView and its model.
    void resetTree();

    /// Creates the TreeView, resets the model with the new scenegraph.
    void constructTree();

    FabricCore::Client m_client;
    DFG::DFGWidget *m_dfgWidget;
    SHStates *m_shStates; 
 
    SHGLScene *m_shGLScene;
    SHBaseTreeView *m_shTreeView;
    SHTreeModel *m_treeModel;
    SHTreeComboBox *m_comboBox;
    
    bool m_showOperators;
    bool m_showProperties;
    bool m_bUpdatingSelection;
};

} // namespace SceneHub
} // namespace FabricUI

#endif //__UI_SCENEHUB_BASE_TREE_VIEW_MANAGER_H__
