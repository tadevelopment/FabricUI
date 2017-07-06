//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValAnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;

size_t RTValAnimXFCurveConstModel::getHandleCount() const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  return const_cast<FabricCore::RTVal*>(&m_val)->callMethod( "UInt32", "keyframeCount", 0, NULL ).getUInt32();
}

Handle RTValAnimXFCurveConstModel::getHandle( size_t i ) const
{
  const size_t argc = 2;
  FabricCore::RTVal args[argc] = {
    FabricCore::RTVal::ConstructSInt32( m_val.getContext(), i ),
    FabricCore::RTVal::Construct( m_val.getContext(), "AnimX::Keyframe", 0, NULL )
  };
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "Boolean", "keyframeAtIndex", argc, args );
  FabricCore::RTVal key = args[1];
  Handle dst;
  dst.pos.setX( key.maybeGetMember( "time" ).getFloat64() );
  dst.pos.setY( key.maybeGetMember( "value" ).getFloat64() );
  dst.tanIn.setX( key.maybeGetMember( "tanIn" ).maybeGetMember( "x" ).getFloat64() );
  dst.tanIn.setY( key.maybeGetMember( "tanIn" ).maybeGetMember( "y" ).getFloat64() );
  dst.tanOut.setX( key.maybeGetMember( "tanOut" ).maybeGetMember( "x" ).getFloat64() );
  dst.tanOut.setY( key.maybeGetMember( "tanOut" ).maybeGetMember( "y" ).getFloat64() );
  return dst;
}

void RTValAnimXFCurveModel::setHandle( size_t i, Handle h )
{
  const size_t argc = 9;
  FabricCore::RTVal args[argc] =
  {
    FabricCore::RTVal::ConstructUInt32( m_val.getContext(), i ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.pos.x() ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.pos.y() ),
    FabricCore::RTVal::ConstructSInt32( m_val.getContext(), 9 ), // HACK : TODO
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanIn.x() ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanIn.y() ),
    FabricCore::RTVal::ConstructSInt32( m_val.getContext(), 9 ), // HACK : TODO
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanOut.x() ),
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), h.tanOut.y() )
  };
  m_val.callMethod( "", "setKeyframe", argc, args );
  emit this->handleMoved( i );
}

qreal RTValAnimXFCurveConstModel::evaluate( qreal v ) const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  FabricCore::RTVal time = FabricCore::RTVal::ConstructFloat64( m_val.getContext(), v );
  return const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "Float64", "evaluate", 1, &time ).getFloat64();
}

void RTValAnimXFCurveConstModel::setValue( FabricCore::RTVal v )
{
  const size_t previousHc = this->getHandleCount();
  // TODO : remove extra handles
  m_val = v;
  const size_t hc = this->getHandleCount();
  for( size_t i = previousHc; i < hc; i++ )
    emit this->handleAdded();
  for( size_t i = 0; i < hc; i++ )
    emit this->handleMoved( i );
}

void RTValAnimXFCurveModel::addHandle()
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  m_val.callMethod( "", "pushKeyframe", 0, NULL );
  emit this->handleAdded();
}

void RTValAnimXFCurveModel::deleteHandle( size_t i )
{
  assert( m_val.isValid() );
  FabricCore::RTVal index = FabricCore::RTVal::ConstructSInt32( m_val.getContext(), i );
  m_val.callMethod( "", "removeKeyframe", 1, &index );
  emit this->handleDeleted( i );
}
