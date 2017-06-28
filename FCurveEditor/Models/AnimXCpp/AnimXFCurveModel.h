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

  std::vector<adsk::Keyframe> m_keys;

public:

  void addHandle( const Handle& );

  // AbstractFCurveModel
  size_t getHandleCount() const FTL_OVERRIDE { return m_keys.size(); }
  Handle getHandle( size_t ) const FTL_OVERRIDE;
  void setHandle( size_t, Handle ) FTL_OVERRIDE;
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
