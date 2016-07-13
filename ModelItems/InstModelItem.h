//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTMODELITEM_H
#define FABRICUI_MODELITEMS_INSTMODELITEM_H

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
    class InstModelItem : public ItemModelItem
    {
    public:
      
      InstModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName
        );
      ~InstModelItem();

      virtual bool isInst() const { return true; }

      virtual int getNumChildren() /*override*/;
      virtual FTL::CStrRef getChildName( int i ) /*override*/;
      
      virtual FabricUI::ValueEditor::BaseModelItem *
      createChild( FTL::StrRef name ) /*override*/;

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

#endif // FABRICUI_MODELITEMS_INSTMODELITEM_H
