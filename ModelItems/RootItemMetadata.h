//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_ROOTITEMMETADATA_H
#define FABRICUI_MODELITEMS_ROOTITEMMETADATA_H

#include <FabricUI/ModelItems/DFGModelItemMetadata.h>

namespace FabricUI {
namespace ModelItems {

class RootItemMetadata : public DFGModelItemMetadata
{
public:

  RootItemMetadata( RootModelItem *rootModelItem )
    : DFGModelItemMetadata()
    {}

  virtual const char* getString( const char* key ) const /*override*/
  {
    if ( key == VEExpandedKey )
      return "1";

    return ItemMetadata::getString( key );
  }
};

} // namespace ModelItems
} // namespace FabricUI

#endif // FABRICUI_MODELITEMS_ROOTITEMMETADATA_H
