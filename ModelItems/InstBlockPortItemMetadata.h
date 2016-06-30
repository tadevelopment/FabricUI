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
        return ItemPortItemMetadata::getString( key );
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTBLOCKPORTITEMMETADATA_H
