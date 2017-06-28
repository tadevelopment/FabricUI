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

class RTValAnimXFCurveModel : public AbstractFCurveModel
{
  Q_OBJECT

public:
  size_t getHandleCount() const FTL_OVERRIDE { return 0; }
  Handle getHandle( size_t ) const FTL_OVERRIDE { return Handle(); }
  void setHandle( size_t, Handle ) FTL_OVERRIDE {}
  qreal evaluate( qreal v ) const { return 0; }
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEMODEL_H
