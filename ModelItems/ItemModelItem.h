//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_ITEMMODELITEM_H
#define FABRICUI_MODELITEMS_ITEMMODELITEM_H

#include <FabricCore.h>
#include <FabricUI/ModelItems/RootModelItem.h>
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
    class ItemModelItem : public RootModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_itemPath;
  
    public:
      
      ItemModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef itemPath
        );
      ~ItemModelItem();

      virtual bool isItem() const /*override*/ { return true; }
      virtual bool isRef() const /*override*/ { return false; }

      FTL::CStrRef getItemPath()
        { return m_itemPath; }

      virtual int getNumChildren() /*override*/;
      virtual FTL::CStrRef getChildName( int i ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;

      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newName ) /*override*/;

      virtual void onRenamed(
        FTL::CStrRef oldItemPath,
        FTL::CStrRef newItemPath
        ) /*override*/;

      // Metadata

      virtual FabricUI::ValueEditor::ItemMetadata* getMetadata();
      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_ITEMMODELITEM_H
