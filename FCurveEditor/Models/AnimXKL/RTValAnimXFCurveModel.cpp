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

FabricCore::RTVal RTValAnimXFCurveConstModel::idToIndex( size_t i ) const
{
  FabricCore::RTVal bRV = FabricCore::RTVal::ConstructBoolean( m_val.getContext(), true );
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "", "useIds", 1, &bRV );
  FabricCore::RTVal iRV = FabricCore::RTVal::ConstructUInt32( m_val.getContext(), i );
  return const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "UInt32", "getKeyIndex", 1, &iRV );
}

Handle RTValAnimXFCurveConstModel::getHandle( size_t i ) const
{
  const size_t argc = 2;
  FabricCore::RTVal args[argc] = {
    this->idToIndex( i ),
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

qreal RTValAnimXFCurveConstModel::evaluate( qreal v ) const
{
  if( !m_val.isValid() || m_val.isNullObject() )
    return 0;
  FabricCore::RTVal time = FabricCore::RTVal::ConstructFloat64( m_val.getContext(), v );
  return const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "Float64", "evaluate", 1, &time ).getFloat64();
}

void RTValAnimXFCurveVersionedConstModel::update( bool emitChanges ) const
{
  const bool invalidVal = ( !m_val.isValid() || m_val.isNullObject() );
  size_t sVersion = invalidVal ? 0 : const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "UInt32", "getStructureVersion", 0, NULL ).getUInt32();
  size_t vVersion = invalidVal ? 0 : const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "UInt32", "getValueVersion", 0, NULL ).getUInt32();

  const bool structureChanged = ( m_lastStructureVersion != sVersion );
  const bool valueChanged = ( m_lastValueVersion != vVersion );
  const size_t lastHandleCount = m_lastHandleCount;

  m_lastStructureVersion = sVersion;
  m_lastValueVersion = vVersion;
  m_lastHandleCount = Parent::getHandleCount();

  if( !emitChanges )
    return;

  const size_t hc = Parent::getHandleCount();
  if( structureChanged )
  {
    for( size_t i = lastHandleCount; i > hc; i-- )
      emit this->handleDeleted( i-1 );
    for( size_t i = lastHandleCount; i < hc; i++ )
      emit this->handleAdded();
  }
  else
    assert( lastHandleCount == hc );
    
  if( valueChanged )
  {
    for( size_t i = 0; i < hc; i++ )
      emit this->handleMoved( i );
  }
}

inline void SetHandle( FabricCore::RTVal& m_val, FabricCore::RTVal& index, Handle h )
{
  const size_t argc = 9;
  FabricCore::RTVal args[argc] =
  {
    index,
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
}

void RTValAnimXFCurveVersionedModel::setHandle( size_t i, Handle h )
{
  SetHandle( m_val, this->idToIndex( i ), h );
  emit this->dirty();
}

void RTValAnimXFCurveModel::setHandle( size_t i, Handle h )
{
  SetHandle( m_val, this->idToIndex( i ), h );
  emit this->handleMoved( i );
}

inline void AddHandle( FabricCore::RTVal& m_val )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  m_val.callMethod( "", "pushKeyframe", 0, NULL );
}

void RTValAnimXFCurveVersionedModel::addHandle()
{
  AddHandle( m_val );
  emit this->dirty();
}

void RTValAnimXFCurveModel::addHandle()
{
  AddHandle( m_val );
  emit this->handleAdded();
}

inline void DeleteHandle( FabricCore::RTVal& m_val, FabricCore::RTVal& index )
{
  assert( m_val.isValid() );
  m_val.callMethod( "", "removeKeyframe", 1, &index );
}

void RTValAnimXFCurveVersionedModel::deleteHandle( size_t i )
{
  DeleteHandle( m_val, this->idToIndex( i ) );
  emit this->dirty();
}

void RTValAnimXFCurveModel::deleteHandle( size_t i )
{
  DeleteHandle( m_val, this->idToIndex( i ) );
  emit this->handleDeleted( i );
}
