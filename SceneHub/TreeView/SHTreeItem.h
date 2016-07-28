/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEITEM_H__
#define __UI_SCENEHUB_SHTREEITEM_H__

#include <vector>
#include <assert.h>
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <QTreeView>
#include <QAbstractItemModel>
 
namespace FabricUI {
namespace SceneHub {

class SHTreeModel;

class SHTreeItem : public FTL::Shareable {

  /**
    SHTreeItem wrapps SceneHub objects:
      - SGObject
      - SGObjectProperty
      - Generators
    to organize and display them in the treeView.
  */

  public:
    enum ItemType { Object, Property, Operator };

    /// Constructor.
    /// \param model A reference to the DFG::DFGWidget.
    /// \param parentItem A reference to the item parent in the hierarchy.
    /// \param client A reference to the FabricCore::Client.
    SHTreeItem(
      SHTreeModel *model, 
      SHTreeItem *parentItem, 
      FabricCore::Client client);

    /// Gets the item description.
    QString desc();

    /// Sets the item index.
    void setIndex(QModelIndex index);

    /// Gets the item index.
    QModelIndex getIndex();

    /// Sets the item name (displayed in the view).
    void setName(QString name);

    /// Updates the expanded state of this item
    void setExpanded( bool state );

    /// Checks if the item is a SGObject.
    bool isObject() const;
    
    /// Checks if the item is a SGObject's reference.
    bool isReference() const;
    
    /// Checks if the item is 'propagated' (properties inheritances).
    bool isPropagated() const;
    
    /// Checks if the item is 'override' (properties ovveride).
    bool isOverride() const;
    
    /// Checks if the item is a generator.
    bool isGenerator() const;

    /// Gets a the item's parent.
    SHTreeItem *parentItem();

    /// Gets the item number of children.
    int childItemCount();

    /// Gets the item child at this row.
    SHTreeItem *childItem(int row);

    /// Retrieves the row of this child.
    int childRow(SHTreeItem *childItem);

    /// Gets the SGObject owned by the item,
    FabricCore::RTVal getSGObject();
    
    /// Gets the SGObjectProperty owned by the item,
    FabricCore::RTVal getSGObjectProperty();
        
    /// Force updating the item.
    void updateNeeded();

    /// Updates the item.
    void updateNeeded(FabricCore::RTVal treeViewObjectData, bool invalidate);

    /// Updates the item children.
    /// Called when expanding the parent.
    void updateChildItemsIfNeeded();

    /// Loads the item hierarchy.
    void loadRecursively();

   
  protected:
    /// Checks if the item is a SGObject.
    void updateChildItemIfNeeded(int row);

    /// Checks if the item is a SGObject.
    SHTreeItem *getOrCreateChildItem(int row);


  private:
    struct ChildItem {
      bool m_updateNeeded;
      FTL::SharedPtr<SHTreeItem> m_child;
      ChildItem() : m_updateNeeded(true) {}
    };

    typedef std::vector<ChildItem> ChildItemVec;

    SHTreeModel *m_model;
    SHTreeItem *m_parentItem;
    FabricCore::Client m_client;

    QString m_name;
    QModelIndex m_index;
    ChildItemVec m_childItems;
    FabricCore::RTVal m_treeViewObjectDataRTVal;

    bool m_needsUpdate;
    bool m_hadInitialUpdate;
    bool m_isPropagated;
    bool m_isOverride;
    bool m_isReference;
    bool m_isGenerator;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_SHTREEITEM_H__
