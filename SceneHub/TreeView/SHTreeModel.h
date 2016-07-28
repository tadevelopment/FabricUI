/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEMODEL_H__
#define __UI_SCENEHUB_SHTREEMODEL_H__

#include <vector>
#include <assert.h>
#include "SHTreeItem.h"
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <QMenu>
#include <QTreeView>
#include <QAbstractItemModel>

 
namespace FabricUI {
namespace SceneHub {

class SHTreeModel : public QAbstractItemModel {

  /**
    SHTreeModel specializes the QtCore::QAbstractItemModel  
    to properly displayed the SceneHuib objects.

    The model has the following structure:
      - root
        - SGObjects:
          - SGProperty
          - generators/operators
    
    The SGProperties as the generators/operators can be shown/hidden.
  */

  Q_OBJECT

  friend class SHTreeItem;

  typedef std::vector<SHTreeItem*> RootItemsVec;


  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param sceneGraph A reference to the QObject parent.
    /// \param parent A reference to the QObject parent (can be null).
    SHTreeModel(
      FabricCore::Client client, 
      FabricCore::RTVal sceneGraph, 
      QObject *parent = 0);

    /// Destructor.
    virtual ~SHTreeModel();

    /// Adds a root to the model.
    QModelIndex addRootItem(FabricCore::RTVal rootSGObject);

    /// Shows the SGObjectProperties in the TreeView.
    void setShowProperties(bool show);

    /// Shows the generators/operators in the TreeView.
    void setShowOperators(bool show);

    /// Sets the SGObjectProperties color in the TreeView.
    void setPropertyColor(QColor color);
    
    /// Sets the SGObject color in the TreeView if it's a reference.
    void setReferenceColor(QColor color);
    
    /// Sets the canvas operator color in the TreeView.
    void setOperatorColor(QColor color);

    /// Gets the indexes (SGObject, SGObjectProperty, CanvasOperators) 
    /// of the sgObject in the TreeView. 
    std::vector<QModelIndex> getIndicesFromSGObject(FabricCore::RTVal sgObject);

    /// Implementation of QtGui::QAbstractItemModel.
    virtual QVariant data(const QModelIndex &index, int role) const;

    /// Implementation of QtGui::QAbstractItemModel.
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    /// Implementation of QtGui::QAbstractItemModel.
    virtual QModelIndex index(
      int row, 
      int col, 
      const QModelIndex &parentIndex = QModelIndex()) const;
    
    /// Implementation of QtGui::QAbstractItemModel.
    virtual QModelIndex parent(const QModelIndex &childIndex) const;

    /// Implementation of QtGui::QAbstractItemModel.
    virtual int rowCount(const QModelIndex &index) const;

    /// Implementation of QtGui::QAbstractItemModel.
    virtual int columnCount(const QModelIndex &index) const;

    /// Safely signal emission when the scene hierachy changed.
    void emitSceneHierarchyChanged();

    /// Safely signal emission when the scene changed.
    void emitSceneChanged();

    /// \internal
    class SceneHierarchyChangedBlocker  {
      public:
        SceneHierarchyChangedBlocker(SHTreeModel *model) : m_model(model) {
          if(m_model->m_sceneHierarchyChangedBlockCount++ == 0)
            m_model->m_sceneHierarchyChangedPending = false;
        }

        ~SceneHierarchyChangedBlocker() {
          if(--m_model->m_sceneHierarchyChangedBlockCount == 0
            && m_model->m_sceneHierarchyChangedPending) {
            emit m_model->sceneHierarchyChanged();
          }
        }

      private:
        SHTreeModel *m_model;
    };


  signals:
    /// Emitted when the scene structure changed.
    /// Used to synchronize with the TreeView-3Dview.
    void sceneHierarchyChanged();
    
    /// Emitted when the scene changed (selection).
    /// Used to synchronize with the TreeView-3Dview.
    void sceneChanged();


  public slots:
    /// Updates the model when the scene changed.
    /// Update from 3Dview.
    void onSceneHierarchyChanged();

    
  private:
    RootItemsVec m_rootItems;
    FabricCore::Client m_client;
    FabricCore::RTVal m_treeViewDataRTVal;

    bool m_showProperties;
    bool m_showOperators;

    QVariant m_propertyColorVariant;
    QVariant m_referenceColorVariant;
    QVariant m_operatorColorVariant;
    QVariant m_propagatedFontVariant;
    QVariant m_overrideFontVariant;
    QVariant m_overridePropagatedFontVariant;

    FabricCore::RTVal m_getUpdatedChildDataArgs[7];
    FabricCore::RTVal m_updateArgs[3];

    unsigned int m_sceneHierarchyChangedBlockCount;
    bool m_sceneHierarchyChangedPending;
};

} // namespace SceneHub
} // namespace FabricUI

#endif// __UI_SCENEHUB_SHTREEMODEL_H__
