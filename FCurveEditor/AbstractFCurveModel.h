//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H
#define FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H

#include <QObject>
#include <QPoint>

namespace FabricUI
{
namespace FCurveEditor
{

struct Handle
{
  QPointF pos;
  QPointF tanIn, tanOut;
};

class AbstractFCurveModel : public QObject
{
  Q_OBJECT

public:

  virtual size_t getHandleCount() const = 0;
  virtual Handle getHandle( size_t ) const = 0;
  virtual void setHandle( size_t, Handle ) {}

  virtual qreal evaluate( qreal v ) const = 0;
  // TODO : evaluate arrays, to reduce the overhead

signals:
  void handleAdded() const;
  void handleMoved( size_t ) const;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H
