//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTBLOCKPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_INSTBLOCKPORTITEMMETADATA_H

#include <FabricCore.h>
#include <FabricUI/ModelItems/ItemPortItemMetadata.h>
#include <FabricUI/ModelItems/InstBlockPortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class InstBlockPortItemMetadata : public ItemPortItemMetadata
    {
    public:

      InstBlockPortItemMetadata( InstBlockPortModelItem *instBlockPortModelItem )
        : ItemPortItemMetadata( instBlockPortModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        char const *res = ItemPortItemMetadata::getString( key );
        if ( !res || !res[0] )
        {
          // If we don't have a result, we check the same port on the execBlock
          FabricCore::DFGExec exec = m_nodePortModelItem->getExec();
          FTL::CStrRef portPath = m_nodePortModelItem->getPortPath();
          res = exec.getPortModelMetadata( portPath.c_str(), key );
        }
        return res;
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTBLOCKPORTITEMMETADATA_H
