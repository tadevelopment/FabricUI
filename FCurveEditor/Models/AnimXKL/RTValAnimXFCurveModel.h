//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H
#define FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <FTL/Config.h>
#include <FabricCore.h>

namespace FabricUI
{
namespace FCurveEditor
{

class RTValAnimXFCurveConstModel : public AbstractFCurveModel
{
  Q_OBJECT

protected:
  FabricCore::RTVal m_val;

  FabricCore::RTVal idToIndex( size_t ) const;

public:
  size_t getHandleCount() const FTL_OVERRIDE;
  Handle getHandle( size_t ) const FTL_OVERRIDE;
  qreal evaluate( qreal v ) const FTL_OVERRIDE;

  void setValue( FabricCore::RTVal );
  inline FabricCore::RTVal value() { return m_val; }
};

class RTValAnimXFCurveModel : public RTValAnimXFCurveConstModel
{
  Q_OBJECT

public:
  void setHandle( size_t, Handle ) FTL_OVERRIDE;
  void addHandle() FTL_OVERRIDE;
  void deleteHandle( size_t ) FTL_OVERRIDE;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H
