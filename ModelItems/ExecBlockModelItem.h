//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_EXECBLOCKMODELITEM_H
#define FABRICUI_MODELITEMS_EXECBLOCKMODELITEM_H

#include <FabricCore.h>
#include <FabricUI/ModelItems/ItemModelItem.h>
#include <FTL/StrRef.h>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{
namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    class ExecBlockModelItem : public ItemModelItem
    {
    public:
      
      ExecBlockModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef execBlockName
        );
      ~ExecBlockModelItem();

      virtual FabricUI::ValueEditor::BaseModelItem *
      createChild( FTL::StrRef name );

      virtual QVariant getValue();

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_EXECBLOCKMODELITEM_H
