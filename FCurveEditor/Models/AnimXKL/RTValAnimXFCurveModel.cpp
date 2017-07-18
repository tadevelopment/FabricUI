//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValAnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;

size_t RTValAnimXFCurveConstModel::getKeyCount() const
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
Key RTValAnimXFCurveConstModel::getOrderedKey( size_t index ) const
{
  const size_t argc = 2;
  FabricCore::RTVal args[argc] = {
    FabricCore::RTVal::ConstructUInt32( m_val.getContext(), index ),
    FabricCore::RTVal::Construct( m_val.getContext(), "AnimX::Keyframe", 0, NULL )
  };
  const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "Boolean", "keyframeAtIndex", argc, args );
  FabricCore::RTVal key = args[1];
  Key dst;
  dst.pos.setX( key.maybeGetMember( "time" ).getFloat64() );
  dst.pos.setY( key.maybeGetMember( "value" ).getFloat64() );
  dst.tanIn.setX( key.maybeGetMember( "tanIn" ).maybeGetMember( "x" ).getFloat64() );
  dst.tanIn.setY( key.maybeGetMember( "tanIn" ).maybeGetMember( "y" ).getFloat64() );
  dst.tanOut.setX( key.maybeGetMember( "tanOut" ).maybeGetMember( "x" ).getFloat64() );
  dst.tanOut.setY( key.maybeGetMember( "tanOut" ).maybeGetMember( "y" ).getFloat64() );
  return dst;
}

Key RTValAnimXFCurveConstModel::getKey( size_t id ) const
{
  return this->getOrderedKey( this->idToIndex( id ).getUInt32() );
}

size_t RTValAnimXFCurveConstModel::getIndexAfterTime( qreal time ) const
{
  const size_t argc = 2;
  FabricCore::RTVal args[argc] = {
    FabricCore::RTVal::ConstructFloat64( m_val.getContext(), time ),
    FabricCore::RTVal::ConstructUInt32( m_val.getContext(), 0 )
  };
  FabricCore::RTVal b = const_cast<FabricCore::RTVal*>( &m_val )
    ->callMethod( "Boolean", "keyframeIndex", argc, args );
  assert( b.getBoolean() );
  return args[1].getUInt32();
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
  const size_t lastKeyCount = m_lastKeyCount;

  m_lastStructureVersion = sVersion;
  m_lastValueVersion = vVersion;
  m_lastKeyCount = Parent::getKeyCount();

  if( !emitChanges )
    return;

  const size_t hc = Parent::getKeyCount();
  if( structureChanged )
  {
    for( size_t i = lastKeyCount; i > hc; i-- )
      emit this->keyDeleted( i-1 );
    for( size_t i = lastKeyCount; i < hc; i++ )
      emit this->keyAdded();
  }
  else
    assert( lastKeyCount == hc );
    
  if( valueChanged )
  {
    for( size_t i = 0; i < hc; i++ )
      emit this->keyMoved( i );
  }
}

inline void SetKey( FabricCore::RTVal& m_val, const FabricCore::RTVal& index, Key h )
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

void RTValAnimXFCurveVersionedModel::setKey( size_t i, Key h )
{
  SetKey( m_val, this->idToIndex( i ), h );
  emit this->dirty();
}

void RTValAnimXFCurveModel::setKey( size_t i, Key h )
{
  SetKey( m_val, this->idToIndex( i ), h );
  emit this->keyMoved( i );
}

inline void AddKey( FabricCore::RTVal& m_val )
{
  assert( m_val.isValid() );
  if( m_val.isNullObject() )
    m_val = FabricCore::RTVal::Create( m_val.getContext(), "AnimX::AnimCurve", 0, NULL );
  m_val.callMethod( "", "pushKeyframe", 0, NULL );
}

void RTValAnimXFCurveVersionedModel::addKey()
{
  AddKey( m_val );
  emit this->dirty();
}

void RTValAnimXFCurveModel::addKey()
{
  AddKey( m_val );
  emit this->keyAdded();
}

inline void DeleteKey( FabricCore::RTVal& m_val, FabricCore::RTVal* index )
{
  assert( m_val.isValid() );
  m_val.callMethod( "", "removeKeyframe", 1, index );
}

void RTValAnimXFCurveVersionedModel::deleteKey( size_t i )
{
  FabricCore::RTVal index = this->idToIndex( i );
  DeleteKey( m_val, &index );
}

void RTValAnimXFCurveModel::deleteKey( size_t i )
{
  FabricCore::RTVal index = this->idToIndex( i );
  DeleteKey( m_val, &index );
  emit this->keyDeleted( i );
}
