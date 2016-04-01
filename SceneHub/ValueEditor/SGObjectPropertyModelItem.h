//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/SceneHub/Commands/SHCmdViewWidget.h>
#include <FTL/StrRef.h>

namespace FabricUI
{

  namespace SceneHub
  {

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing propertyes
    class SGObjectPropertyModelItem : public BaseModelItem
    {
      Q_OBJECT
      
    protected:

      SHCmdViewWidget * m_cmdViewWidget;
      FabricCore::Client m_client;
      FabricCore::RTVal m_rtVal;
      std::string m_name;
      std::string m_rtValType;
      int m_lastValueVersion;
      bool m_rootItem;

    public:

      SGObjectPropertyModelItem(
        SHCmdViewWidget * cmdViewWidget,
        FabricCore::Client client,
        FabricCore::RTVal rtVal,
        bool isRootItem
        );
      ~SGObjectPropertyModelItem();

      virtual bool isSGObjectProperty() const /*override*/ { return true; }

      BaseModelItem *createChild( FTL::CStrRef name ) /*override*/;

      virtual int getNumChildren() /*override*/;

      virtual FTL::CStrRef getChildName( int i ) /*override*/;

      // Detects changes for this SGObjectProperty and updates the value.
      void updateFromScene();

      const FabricCore::RTVal& getSGObjectProperty() { return m_rtVal; }

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
      // Value
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getRTValType();

      virtual QVariant getValue() /*override*/;

      virtual bool hasDefault() /*override*/;
      
      virtual void resetToDefault() /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Metadata
      /////////////////////////////////////////////////////////////////////////

      virtual ItemMetadata* getMetadata() /*override*/;

      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}
