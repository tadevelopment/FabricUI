//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTBLOCKPORTMODELITEM_H
#define FABRICUI_MODELITEMS_INSTBLOCKPORTMODELITEM_H

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

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class InstBlockPortModelItem : public ItemPortModelItem
    {
    public:

      InstBlockPortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef instBlockPath,
        FTL::StrRef instBlockPortName
        );
      ~InstBlockPortModelItem();

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;

      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newInstBlockPortName ) /*override*/;

      virtual void onRenamed(
        FTL::CStrRef oldInstBlockPortName,
        FTL::CStrRef newInstBlockPortName
        ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Others
      /////////////////////////////////////////////////////////////////////////

      virtual FabricUI::ValueEditor::ItemMetadata *getMetadata() /*override*/;

      virtual bool hasDefault() /*override*/;
      virtual void resetToDefault() /*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTBLOCKPORTMODELITEM_H
