//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ModelItems/RootModelItem.h>

namespace FabricUI
{
  namespace SceneHub
  {
    //////////////////////////////////////////////////////////////////////////
    class SGObjectModelItem : public ModelItems::RootModelItem
    {
      Q_OBJECT
      
    private:

      FabricCore::Client m_client;
      FabricCore::RTVal m_rtVal;
      std::string m_name;
      FabricCore::RTVal m_propertiesRtVal;
      std::map<std::string, unsigned int> m_propertyNameMap;

    public:

      SGObjectModelItem(
        FabricCore::Client client,
        FabricCore::RTVal rtVal
        );
      ~SGObjectModelItem();

      virtual bool isSGObject() const /*override*/ { return true; }

      BaseModelItem *createChild( FTL::CStrRef name ) /*override*/;

      virtual int getNumChildren() /*override*/;

      virtual FTL::CStrRef getChildName( int i ) /*override*/;

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
      virtual QVariant getValue() /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Metadata
      /////////////////////////////////////////////////////////////////////////

      virtual ItemMetadata* getMetadata() /*override*/;

      virtual void setMetadataImp( const char* key, 
                                const char* value, 
                                bool canUndo )/*override*/;

    signals:
      void propertyItemInserted( BaseModelItem * item );

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;

      void ensurePropertiesRTVal();
    };
  }
}
