//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_VARPORTMODELITEM_H
#define FABRICUI_MODELITEMS_VARPORTMODELITEM_H

#include <FabricUI/ModelItems/ItemPortModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>

namespace FabricUI
{

namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {

    class ItemPortItemMetadata;

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class VarPortModelItem : public ItemPortModelItem
    {
    public:

      VarPortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        FTL::StrRef portName
        );
      ~VarPortModelItem();

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;

      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newName ) /*override*/;
      
      virtual void onRenamed(
        FTL::CStrRef oldName,
        FTL::CStrRef newName
        ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Others
      /////////////////////////////////////////////////////////////////////////

      virtual QVariant getValue();

      virtual bool hasDefault() /*override*/;
      virtual void resetToDefault() /*override*/;

      virtual FabricUI::ValueEditor::ItemMetadata* getMetadata();
    };
  }
}

#endif // FABRICUI_MODELITEMS_VARPORTMODELITEM_H
