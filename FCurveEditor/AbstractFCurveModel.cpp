//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>

using namespace FabricUI::FCurveEditor;

void AbstractFCurveModel::moveHandles( const size_t* indices, const size_t nbIndices, QPointF delta )
{
  for( size_t i = 0; i < nbIndices; i++ )
  {
    Handle h = getHandle( indices[i] );
    h.pos += delta;
    this->setHandle( indices[i], h );
  }
}

void AbstractFCurveModel::deleteHandles( const size_t* indices, const size_t nbIndices )
{
  for( size_t i = nbIndices; i > 0; i-- )
    this->deleteHandle( indices[i-1] );
}
