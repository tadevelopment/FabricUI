#include "AnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;
using namespace adsk;

Key AnimxFCurveModel::getOrderedKey( size_t index ) const
{
  assert( index < m_keys.size() );
  const Keyframe& key = m_keys[index].key;
  Key dst;
  dst.pos = QPointF( key.time, key.value );
  dst.tanIn = QPointF( key.tanIn.x, key.tanIn.y );
  dst.tanOut = QPointF( key.tanOut.x, key.tanOut.y );
  return dst;
}

Key AnimxFCurveModel::getKey( size_t uiId ) const
{
  assert( uiId < m_keys.size() );
  return this->getOrderedKey( m_uiIdToIndex[uiId] );
}

void AnimxFCurveModel::setKey( size_t uiId, Key h )
{
  assert( uiId < m_keys.size() );
  const size_t i = m_uiIdToIndex[uiId];
  Keyframe& key = m_keys[i].key;
  key.time = h.pos.x();
  key.value = h.pos.y();
  key.tanIn.x = h.tanIn.x();
  key.tanIn.y = h.tanIn.y();
  key.tanOut.x = h.tanOut.x();
  key.tanOut.y = h.tanOut.y();
  // TODO : tangent types
  //key.tanIn.type = adsk::TangentType::Parabolic;
  //key.tanOut.type = adsk::TangentType::Parabolic;
  key.linearInterpolation = false; // HACK/TODO
  key.index = i;

  // Reordering the keys
#if 1
  {
    size_t disorderIndex = i;
    while( disorderIndex > 0 && m_keys[disorderIndex - 1].key.time > m_keys[disorderIndex].key.time )
    {
      this->swap( disorderIndex - 1, disorderIndex );
      disorderIndex--;
    }
    while( disorderIndex < m_keys.size() - 1 && m_keys[disorderIndex].key.time > m_keys[disorderIndex + 1].key.time )
    {
      this->swap( disorderIndex, disorderIndex + 1 );
      disorderIndex++;
    }
  }
#else
  { // doesn't scale well with the number of key (only for debug)
    std::sort( m_keys.begin(), m_keys.end(), []( const UIKey& a, const UIKey& b ) { return a.key.time < b.key.time; } );
    for( size_t i = 0; i < m_keys.size(); i++ )
      m_uiIdToIndex[m_keys[i].uiId] = i;
  }
#endif

  emit this->keyMoved( uiId );
}

void AnimxFCurveModel::addKey()
{
  UIKey key;
  key.key = adsk::Keyframe();
  key.uiId = m_keys.size();
  m_keys.push_back( key );
  m_uiIdToIndex.push_back( m_keys.size()-1 );
  emit this->keyAdded();
}

void AnimxFCurveModel::addKey( const Key& h )
{
  this->addKey();
  this->setKey( m_keys.size() - 1, h );
}

void AnimxFCurveModel::deleteKey( size_t uiId )
{
  size_t index = m_uiIdToIndex[uiId];

  // TODO : more efficient way to delete keys ?

  // Shifting the local indices
  for( size_t i = index; i < m_keys.size()-1; i++ )
  {
    m_keys[i] = m_keys[i + 1];
    m_keys[i].key.index--;
    m_uiIdToIndex[m_keys[i].uiId] = i;
  }
  m_keys.resize( m_keys.size() - 1 );

  // Shifting the UI indices
  for( size_t i = uiId; i < m_uiIdToIndex.size() - 1; i++ )
  {
    m_uiIdToIndex[i] = m_uiIdToIndex[i + 1];
    m_keys[m_uiIdToIndex[i]].uiId = i;
  }
  m_uiIdToIndex.resize( m_uiIdToIndex.size() - 1 );

  emit this->keyDeleted( uiId );
}


void AnimxFCurveModel::autoTangents( size_t uiId )
{
  if( m_keys.size() == 1 )
    return;
  const size_t index = m_uiIdToIndex[uiId];
  for( size_t dir = 0; dir <= 1; dir++ )
    adsk::autoTangent(
      dir == 0,
      m_keys[index].key,
      ( index > 0 ? &m_keys[index - 1].key : NULL ),
      ( index < m_keys.size()-1 ? &m_keys[index + 1].key : NULL ),
      adsk::CurveInterpolatorMethod::Bezier, // TODO expose as parameter
      ( dir == 0 ? m_keys[index].key.tanIn.x : m_keys[index].key.tanOut.x ),
      ( dir == 0 ? m_keys[index].key.tanIn.y : m_keys[index].key.tanOut.y )
    );
  emit this->keyMoved( uiId );
}

qreal AnimxFCurveModel::evaluate( qreal v ) const
{
  return evaluateCurve( v, *this );
}

bool AnimxFCurveModel::keyframeAtIndex( int i, adsk::Keyframe& k ) const
{
  if( i >= 0 && i < m_keys.size() )
  {
    k = m_keys[i].key;
    return true;
  }
  return false;
}

bool AnimxFCurveModel::keyframe( double time, adsk::Keyframe& key ) const
{
  if( m_keys.size() == 0 )
    return false;

  // TODO : implement this efficiently (binary search tree ?)
  size_t closestI = m_keys.size() - 1;
  double closestT = INFINITY;
  for( size_t i = 0; i < m_keys.size(); i++ )
  {
    if( m_keys[i].key.time >= time && m_keys[i].key.time < closestT )
    {
      closestI = i;
      closestT = m_keys[i].key.time;
    }
  }
  key = m_keys[closestI].key;
  return true;
}

size_t AnimxFCurveModel::getIndexAfterTime( qreal time ) const
{
  adsk::Keyframe key;
  bool valid = this->keyframe( time, key );
  assert( valid );
  return key.index;
}

bool AnimxFCurveModel::first( adsk::Keyframe& k ) const
{
  if( m_keys.size() > 0 )
  {
    k = m_keys[0].key;
    return true;
  }
  return false;
}

bool AnimxFCurveModel::last( adsk::Keyframe& k ) const
{
  if( m_keys.size() > 0 )
  {
    k = m_keys[m_keys.size()-1].key;
    return true;
  }
  return false;
}
