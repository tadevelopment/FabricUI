//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SCENEHUB_SHBASEVEEDITOROWNER_H__
#define __UI_SCENEHUB_SHBASEVEEDITOROWNER_H__


#include "SGObjectModelItem.h"
#include "SGObjectPropertyModelItem.h"
#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/DFG/SHDFGBinding.h>

namespace FabricUI {
namespace SceneHub {

class SHBaseVEEditorOwner : public DFG::DFGVEEditorOwner 
{

  /**
    SHBaseVEEditorOwner spcialized the DFG::DFGVEEditorOwner.
    It defines a base class to the SHVEEditorOwner defined in C++ and python.

    It defines the base methods to edit the properties of a SceneHub ojbects:
      - SGObjects
      - SGObjectProperty
  */

  Q_OBJECT

  public:
    /// Constructor.
    /// \param dfgWidget A reference to the DFG.DFGWidget
    SHBaseVEEditorOwner(DFG::DFGWidget *dfgWidget);
      
    /// Destructor.
    virtual ~SHBaseVEEditorOwner();

    /// Gets the current SGObjectPropertyModelItem.
    SGObjectPropertyModelItem *getSGObjectPropertyModelItem();


  signals:
    /// Synchronizes the commands.
    /// Shall be emit when a SGObjectProperty changed,
    void synchronizeCommands();


  protected slots:
    /// Synchronized the commands.
    /// Dumb slot to emit the synchronizeCommands signal.
    void onSynchronizeCommands();

    /// Update the valueEditor structure when a property of an object is displayed.
    void onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem * item);
    

  protected:
    /// Casts a ValueEditor::BaseModelItem to a SGObjectModelItem.
    SGObjectModelItem* castToSGModelItem(ValueEditor::BaseModelItem *item);
    
    /// Updates the valueEditor root-item
    void updateSGObject( const FabricCore::RTVal& sgObject );
    
    /// Updates the current SGObjectPropertyModelItem
    void updateSGObjectProperty( const FabricCore::RTVal& sgObjectProperty );

    /// Defined if "root" is a single property
    SGObjectPropertyModelItem *m_objectPropertyItem;
};

} // namespace SceneHub
} // namespace FabricUI

#endif //__UI_SCENEHUB_SHBASEVEEDITOROWNER_H__

