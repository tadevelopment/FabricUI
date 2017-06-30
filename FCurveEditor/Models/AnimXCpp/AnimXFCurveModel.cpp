#include "AnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;
using namespace adsk;

Handle AnimxFCurveModel::getHandle( size_t uiId ) const
{
  assert( uiId < m_keys.size() );
  const Keyframe& key = m_keys[m_uiIdToIndex[uiId]].key;
  Handle dst;
  dst.pos = QPointF( key.time, key.value );
  dst.tanIn = QPointF( key.tanIn.x, key.tanIn.y );
  dst.tanOut = QPointF( key.tanOut.x, key.tanOut.y );
  return dst;
}

void AnimxFCurveModel::setHandle( size_t uiId, Handle h )
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

  emit this->handleMoved( uiId );
}

void AnimxFCurveModel::addHandle()
{
  UIKey key;
  key.key = adsk::Keyframe();
  key.uiId = m_keys.size();
  m_keys.push_back( key );
  m_uiIdToIndex.push_back( m_keys.size()-1 );
  emit this->handleAdded();
}

void AnimxFCurveModel::addHandle( const Handle& h )
{
  this->addHandle();
  this->setHandle( m_keys.size() - 1, h );
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
