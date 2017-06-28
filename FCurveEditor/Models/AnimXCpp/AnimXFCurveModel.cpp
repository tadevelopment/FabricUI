#include "AnimXFCurveModel.h"

#include <assert.h>

using namespace FabricUI::FCurveEditor;
using namespace adsk;

Handle AnimxFCurveModel::getHandle( size_t i ) const
{
  assert( i < m_keys.size() );
  const Keyframe& key = m_keys[i];
  Handle dst;
  dst.pos = QPointF( key.time, key.value );
  dst.tanIn = QPointF( key.tanIn.x, key.tanIn.y );
  dst.tanOut = QPointF( key.tanOut.x, key.tanOut.y );
  return dst;
}

void AnimxFCurveModel::setHandle( size_t i, Handle h )
{
  assert( i < m_keys.size() );
  Keyframe& key = m_keys[i];
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
  emit this->handleMoved( i );
}

void AnimxFCurveModel::addHandle( const Handle& h )
{
  m_keys.push_back( adsk::Keyframe() );
  this->setHandle( m_keys.size() - 1, h );
  emit this->handleAdded();
}

qreal AnimxFCurveModel::evaluate( qreal v ) const
{
  return evaluateCurve( v, *this );
}

bool AnimxFCurveModel::keyframeAtIndex( int i, adsk::Keyframe& k ) const
{
  if( i >= 0 && i < m_keys.size() )
  {
    k = m_keys[i];
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
    if( m_keys[i].time >= time && m_keys[i].time < closestT )
    {
      closestI = i;
      closestT = m_keys[i].time;
    }
  }
  key = m_keys[closestI];
  return true;
}

bool AnimxFCurveModel::first( adsk::Keyframe& k ) const
{
  if( m_keys.size() > 0 )
  {
    k = m_keys[0];
    return true;
  }
  return false;
}

bool AnimxFCurveModel::last( adsk::Keyframe& k ) const
{
  if( m_keys.size() > 0 )
  {
    k = m_keys[m_keys.size()-1];
    return true;
  }
  return false;
}
