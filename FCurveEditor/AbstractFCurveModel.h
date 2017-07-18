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
  virtual Handle getOrderedHandle( size_t ) const = 0;
  virtual size_t getIndexAfterTime( qreal ) const = 0;
  virtual void setHandle( size_t, Handle ) {}
  virtual void moveHandles( const size_t* indices, const size_t nbIndices, QPointF delta );
  virtual void addHandle() = 0;
  // Deleting an handle will shift the indices of all the handles after it
  virtual void deleteHandle( size_t ) = 0;
  // the indices will be sorted in increasing order
  virtual void deleteHandles( const size_t* indices, const size_t nbIndices );

  virtual void autoTangents( size_t ) {}

  virtual qreal evaluate( qreal v ) const = 0;
  // TODO : evaluate arrays, to reduce the overhead

  virtual void init() {}
  virtual void update() {}

signals:
  void handleAdded() const;
  void handleDeleted( size_t ) const;
  void handleMoved( size_t ) const;

  // when emitted, UI items should schedule an update
  void dirty() const;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_ABSTRACTFCURVEMODEL_H
