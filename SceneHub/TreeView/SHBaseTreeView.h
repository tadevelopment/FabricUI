/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHBASETREEVIEW_H__
#define __UI_SCENEHUB_SHBASETREEVIEW_H__

#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricUI/SceneHub/SHGLScene.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI {
namespace SceneHub {

class SHBaseTreeView;

class SHTreeView_ViewIndexTarget : public QObject {

  /**
    SHTreeView_ViewIndexTarget synchronizes a SHTreeModel 
    with a QAbstractItemModel.
  */

  Q_OBJECT

  public:
    /// Constructor.
    /// \param shStates A reference to the FabricUI::SceneHub::SHBaseTreeView.
    /// \param QModelIndex The index of the item.
    /// \param parent A reference to the QObject parent.
    SHTreeView_ViewIndexTarget(
      SHBaseTreeView *view, 
      QModelIndex const &index, 
      QObject *parent);

    /// Sets the visibilty of the SGObject.
    /// \param visible Show the object if true.
    /// \param propagationType Local-Propagated-Override
    void setVisibility(bool visible, unsigned char propagationType);


  public slots:
    /// Loads recursively the SGObject.
    void loadRecursively();
    
    /// Expands recursively the SGObject.
    void expandRecursively();

    /// Shows the SGObject.
    void showLocal();
    
    /// Shows the SGObject and its instances.
    void showPropagated();
    
    /// Shows the SGObject override.
    void showOverride();

    /// Hides the SGObject.
    void hideLocal();
    
    /// Hides the SGObject and its instances.
    void hidePropagated();
    
    /// Hides the SGObject override.
    void hideOverride();


  protected:
    /// Expands recursvely the treeItem at index.
    void expandRecursively(QModelIndex const &index);

    /// Reference to the FabricUI::SceneHub::SHBaseTreeView
    SHBaseTreeView *m_view;
    /// QModelIndex to get the SGObject from.
    QModelIndex m_index;
};


class SHBaseTreeView : public QTreeView {
  
  /**
    SHBaseTreeView specializes the QtGui::QTreeView.
    It defines a base class for the SHTreeView defined in C++ and python.
  */

  Q_OBJECT

  /// \internal
  friend class SHTreeView_ViewIndexTarget;

  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param parent A pointor to the QWidget parent.    
    SHBaseTreeView(FabricCore::Client &client, QWidget *parent = 0);

    /// Gets a reference to the FabricCore::Client.
    FabricCore::Client getClient();

    /// Sets the selected treeView items.
    /// Updates from 3DView.
    void setSelectedObjects(FabricCore::RTVal selectedSGObjectArray);

    /// Gets the selected treeView items.
    QModelIndexList getSelectedIndexes();

    /// Gets a SHTreeItem at treeView index.
    static SHTreeItem *GetTreeItemAtIndex(QModelIndex index);


  protected:
    /// Reference to the FabricCore::Client.
    FabricCore::Client m_client;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_SHBASETREEVIEW_H__
