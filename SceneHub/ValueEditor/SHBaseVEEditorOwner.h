//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "SGObjectModelItem.h"
#include "SGObjectPropertyModelItem.h"
#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/DFG/SHDFGBinding.h>

namespace FabricUI {
namespace SceneHub {
    
class SHBaseVEEditorOwner : public DFG::DFGVEEditorOwner {
  Q_OBJECT

  public:
    SHBaseVEEditorOwner(DFG::DFGWidget *dfgWidget);
    
    virtual ~SHBaseVEEditorOwner();

    SGObjectPropertyModelItem *getSGObjectPropertyModelItem();


  signals:
    void synchronizeCommands();


  protected slots:
    void onSynchronizeCommands();

    void onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem * item);
    

  protected:
    SGObjectModelItem* castToSGModelItem(ValueEditor::BaseModelItem *item);

    void updateSGObject( const FabricCore::RTVal& sgObject );
    
    void updateSGObjectProperty( const FabricCore::RTVal& sgObjectProperty );

    SGObjectPropertyModelItem *m_objectPropertyItem;//Defined if "root" is a single property
};

} // namespace SceneHub
} // namespace FabricUI
