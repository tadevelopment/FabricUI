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

  inline void setValue( FabricCore::RTVal v ) { m_val = v; }
  inline FabricCore::RTVal value() { return m_val; }
};

class RTValAnimXFCurveVersionedConstModel : public RTValAnimXFCurveConstModel
{
  Q_OBJECT

  typedef RTValAnimXFCurveConstModel Parent;

protected:
  mutable size_t m_lastHandleCount;
  mutable size_t m_lastStructureVersion;
  mutable size_t m_lastValueVersion;

  void update( bool emitChanges ) const;

public:

  inline void update() const { this->update( true ); }
  size_t getHandleCount() const FTL_OVERRIDE { this->update( false ); return Parent::getHandleCount(); }
  Handle getHandle( size_t i ) const FTL_OVERRIDE { this->update(); return Parent::getHandle( i ); }
  qreal evaluate( qreal v ) const FTL_OVERRIDE { this->update(); return Parent::evaluate( v ); }

  RTValAnimXFCurveVersionedConstModel()
    : m_lastHandleCount( 0 )
    , m_lastStructureVersion( 0 )
    , m_lastValueVersion( 0 )
  {}
};

class RTValAnimXFCurveVersionedModel : public RTValAnimXFCurveVersionedConstModel
{
  Q_OBJECT

public:
  void setHandle( size_t, Handle ) FTL_OVERRIDE;
  void addHandle() FTL_OVERRIDE;
  void deleteHandle( size_t ) FTL_OVERRIDE;
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
