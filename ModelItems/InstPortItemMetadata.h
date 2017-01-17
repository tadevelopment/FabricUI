//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H

#include <FabricCore.h>
#include <FabricUI/ModelItems/ItemPortItemMetadata.h>
#include <FabricUI/ModelItems/InstPortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class InstPortItemMetadata : public ItemPortItemMetadata
    {
    public:

      InstPortItemMetadata( InstPortModelItem *instPortModelItem )
        : ItemPortItemMetadata( instPortModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        char const *res = ItemPortItemMetadata::getString( key );
        if ( !res || !res[0] )
        {
          // If we don't have a result, we check the same port on the executable
          FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
          FTL::CStrRef itemPath = m_nodePortModelItem->getItemPath();
          if ( !exec.isExecBlock( itemPath.c_str() ) )
          {
            FTL::CStrRef portName = m_nodePortModelItem->getPortName();
            FabricCore::DFGExec subExec = exec.getSubExec( itemPath.c_str() );
            res = subExec.getExecPortMetadata( portName.c_str(), key );
          }
        }
        return res;
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H
