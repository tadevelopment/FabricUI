//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_ANIMXFCURVEMODEL_H
#define FABRICUI_FCURVEEDITOR_ANIMXFCURVEMODEL_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <FTL/Config.h>

#include <animx.h>
#include <vector>

namespace FabricUI
{
namespace FCurveEditor
{

class AnimxFCurveModel : public AbstractFCurveModel, public adsk::ICurve
{
  Q_OBJECT

  struct UIKey
  {
    adsk::Keyframe key;
    size_t uiId;
  };
  std::vector<UIKey> m_keys;
  std::vector<size_t> m_uiIdToIndex;

  inline void swap( const size_t a, const size_t b )
  {
    UIKey tmp = m_keys[a];
    m_keys[a] = m_keys[b];
    m_keys[b] = tmp;
    m_keys[a].key.index = a;
    m_keys[b].key.index = b;
    m_uiIdToIndex[m_keys[a].uiId] = a;
    m_uiIdToIndex[m_keys[b].uiId] = b;
  }

public:

  void addHandle( const Handle& );

  // AbstractFCurveModel
  size_t getHandleCount() const FTL_OVERRIDE { return m_keys.size(); }
  Handle getHandle( size_t uiId ) const FTL_OVERRIDE;
  void setHandle( size_t uiId, Handle ) FTL_OVERRIDE;
  void addHandle() FTL_OVERRIDE;
  qreal evaluate( qreal v ) const FTL_OVERRIDE;

  // adsk::ICurve
  bool keyframeAtIndex( int, adsk::Keyframe& ) const FTL_OVERRIDE;
  bool keyframe( double, adsk::Keyframe& ) const FTL_OVERRIDE;
  bool first( adsk::Keyframe& ) const FTL_OVERRIDE;
  bool last( adsk::Keyframe& ) const FTL_OVERRIDE;
  adsk::InfinityType preInfinityType() const FTL_OVERRIDE { return adsk::InfinityType::Linear; } // TODO
  adsk::InfinityType postInfinityType() const FTL_OVERRIDE { return adsk::InfinityType::Linear; } // TODO
  bool isWeighted() const FTL_OVERRIDE { return true; } // TODO
  unsigned int keyframeCount() const FTL_OVERRIDE { return m_keys.size(); }
  bool isStatic() const FTL_OVERRIDE { return false; } // TODO
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_ANIMXFCURVEMODEL_H
