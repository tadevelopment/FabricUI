//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_VARPORTITEMMETADATA_H
#define FABRICUI_MODELITEMS_VARPORTITEMMETADATA_H

#include "ItemPortItemMetadata.h"

#include <FabricUI/ModelItems/VarPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

class VarPortItemMetadata : public ItemPortItemMetadata
{
public:

  VarPortItemMetadata( VarPortModelItem *varPortModelItem )
    : ItemPortItemMetadata( varPortModelItem ) {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == VENotInspectableKey )
    {
      return FTL_STR("").c_str();
    }

    return ItemPortItemMetadata::getString( key );
  }
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_VARPORTITEMMETADATA_H
