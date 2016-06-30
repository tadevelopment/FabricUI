//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTBLOCKMODELITEM_H
#define FABRICUI_MODELITEMS_INSTBLOCKMODELITEM_H

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
    class InstBlockModelItem : public RootModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_instName;
      std::string m_instBlockName;
      std::string m_instBlockPath;
  
    public:
      
      InstBlockModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef instName,
        FTL::StrRef instBlockName
        );
      ~InstBlockModelItem();

      virtual bool isInstBlock() const /*override*/ { return true; }

      FTL::CStrRef getInstName()
        { return m_instName; }
      FTL::CStrRef getInstBlockName()
        { return m_instBlockName; }
      FTL::CStrRef getInstBlockPath()
        { return m_instBlockPath; }

      virtual int getNumChildren() /*override*/;
      virtual FTL::CStrRef getChildName( int i ) /*override*/;

      virtual FabricUI::ValueEditor::BaseModelItem* createChild( FTL::CStrRef name );

      virtual QVariant getValue();

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

      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTBLOCKMODELITEM_H
