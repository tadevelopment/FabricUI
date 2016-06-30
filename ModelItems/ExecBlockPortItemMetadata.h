//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H

#include <FabricCore.h>
#include <FabricUI/ModelItems/ItemPortItemMetadata.h>
#include <FabricUI/ModelItems/ExecBlockPortModelItem.h>

namespace FabricUI
{
  namespace ModelItems
  {

    //////////////////////////////////////////////////////////////////////////
    // MetaData specialization allows access to Port metadata
    class ExecBlockPortItemMetadata : public ItemPortItemMetadata
    {
    public:

      ExecBlockPortItemMetadata( ExecBlockPortModelItem *instPortModelItem )
        : ItemPortItemMetadata( instPortModelItem ) {}

      virtual const char* getString( const char* key ) const /*override*/
      {
        return ItemPortItemMetadata::getString( key );
      }
    };
  }
}

#endif // FABRICUI_MODELITEMS_INSTPORTITEMMETADATA_H
