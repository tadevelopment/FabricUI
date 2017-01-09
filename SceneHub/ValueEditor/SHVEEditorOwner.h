//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SCENEHUB_SHVEEDITOROWNER_H__
#define __UI_SCENEHUB_SHVEEDITOROWNER_H__


#include "SGObjectModelItem.h"
#include "SGObjectPropertyModelItem.h"
#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/DFG/SHDFGBinding.h>

namespace FabricUI {
namespace SceneHub {

class SHVEEditorOwner : public DFG::DFGVEEditorOwner {

  /**
    SHVEEditorOwner specializes the DFG::DFGVEEditorOwner.
    It defines the methods to edit the properties of a SceneGraph objects:
      - SGObjects
      - SGObjectProperty
  */

  Q_OBJECT

  public:
    /// Constructor.
    /// \param dfgWidget A reference to the DFGWidget.
    /// \param shStates A reference to the SHStates.
    SHVEEditorOwner(DFG::DFGWidget *dfgWidget, SHStates* shStates);
      
    /// Destructor.
    virtual ~SHVEEditorOwner();

    /// Gets the current SGObjectPropertyModelItem.
    SGObjectPropertyModelItem *getSGObjectPropertyModelItem();
   
    /// Updates the valueEditor root-item.
    void updateSGObject(const FabricCore::RTVal& sgObject);


  public slots:
    /// Updates the valueEditor when the current rootItem properties
    /// are changed by another modules (from the canvas graph, from manipulators...).
    virtual void onStructureChanged();
    
    /// Override, uses to display the selection properties in the valueEditor.
    void onInspectChanged();
   
    /// Updates the valuesEdditor Tree when the root_item changed. 
    void onSceneChanged();
  

  signals:
    /// Synchronizes the commands.
    /// Shall be emit when a SGObjectProperty changed,
    void synchronizeCommands();

    void canvasSidePanelInspectRequested();


  protected slots:
    /// Synchronizes the commands.
    /// Dumb slot to emit the synchronizeCommands signal.
    void onSynchronizeCommands();

    /// Update the valueEditor structure when a property of an object is displayed.
    void onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem *item);
      
    /// Implementation of DFG::DFGVEEditorOwner.
    virtual void onSidePanelInspectRequested();


  protected:
    /// Casts a ValueEditor::BaseModelItem to a SGObjectModelItem.
    /// Helper for Python call.
    SGObjectModelItem* castToSGModelItem(ValueEditor::BaseModelItem *item);
     
    /// Updates the current SGObjectProperty.
    void updateSGObjectProperty(const FabricCore::RTVal& sgObjectProperty);

    SHStates *m_shStates;

    /// Defined if "root" is a single property.
    SGObjectPropertyModelItem *m_objectPropertyItem;
};

} // namespace SceneHub
} // namespace FabricUI

#endif //__UI_SCENEHUB_SHVEEDITOROWNER_H__
