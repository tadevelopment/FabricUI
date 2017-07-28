//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEDFGCONTROLLER_H
#define FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEDFGCONTROLLER_H

#include <FabricUI/FCurveEditor/Models/AnimXKL/RTValAnimXFCurveModel.h>
#include <string>
#include <QString>
#include <QMap>

namespace FabricUI
{
namespace FCurveEditor
{

class RTValAnimXFCurveDFGController : public RTValAnimXFCurveVersionedConstModel
{
  Q_OBJECT

  std::string m_bindingId, m_dfgPortPath;
  size_t m_interactionId;
  bool m_isInteracting;

  QString m_lastCommand;
  QMap<QString, QString> m_lastArgs;

public:

  RTValAnimXFCurveDFGController() : m_isInteracting( false ) {}

  void setPath( const char* bindingId, const char* dfgPortPath );

  void setKey( size_t i, Key h ) FTL_OVERRIDE;

  void moveKeys( const size_t* indices, const size_t nbIndices, QPointF delta ) FTL_OVERRIDE;

  void addKey( Key k, bool useKey );

  void addKey() FTL_OVERRIDE;

  void addKey( Key k ) FTL_OVERRIDE;

  void deleteKey( size_t i ) FTL_OVERRIDE;

  void deleteKeys( const size_t* indices, const size_t nbIndices ) FTL_OVERRIDE;

  void setPreInfinityType( size_t i ) FTL_OVERRIDE;

  void setPostInfinityType( size_t i ) FTL_OVERRIDE;

  inline void emitDirty() { emit this->dirty(); }

public slots:
  void onInteractionBegin();

  void onInteractionEnd();
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RTVALANIMXFCURVEDFGCONTROLLER_H
