//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
#define FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H

#include <QWidget>
#include <FabricUI/FCurveEditor/FCurveItem.h>
#include <FTL/Config.h>

class QGraphicsScene;

namespace FabricUI
{
namespace FCurveEditor
{
class RuledGraphicsView;

class FCurveEditor : public QWidget
{
  Q_OBJECT

  typedef QWidget Parent;

  // relative position of the value editor (negative values will be from the right/bottom)
  Q_PROPERTY( QPoint vePos READ vePos WRITE setVEPos )
  QPoint m_vePos;
  Q_PROPERTY( bool toolBarEnabled READ toolBarEnabled WRITE setToolBarEnabled )
  bool m_toolbarEnabled;

  RuledGraphicsView* m_rview;
  AbstractFCurveModel* m_model;
  QGraphicsScene* m_scene;
  FCurveItem* m_curveItem;
  class KeyValueEditor;
  KeyValueEditor* m_keyValueEditor;
  class ToolBar;
  ToolBar* m_toolBar;
  void veEditFinished( bool isXNotY );
  void updateVEPos();

public:
  FCurveEditor();
  ~FCurveEditor();
  void setModel( AbstractFCurveModel* );
  void frameAllKeys();
  void frameSelectedKeys();
  void setMode( FCurveItem::Mode );

  inline QPoint vePos() const { return m_vePos; }
  inline void setVEPos( const QPoint& p ) { m_vePos = p; this->updateVEPos(); }
  inline bool toolBarEnabled() const { return m_toolbarEnabled; }
  void setToolBarEnabled( bool );

protected:
  void mousePressEvent( QMouseEvent * ) FTL_OVERRIDE;
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;

private slots:
  void onRectangleSelectReleased( const QRectF&, Qt::KeyboardModifiers );
  void onFrameAllKeys();
  void onFrameSelectedKeys();
  void onDeleteSelectedKeys();
  void onEditedKeysChanged();
  void onRepaintViews();
  void setModeSelect() { this->setMode( FCurveItem::SELECT ); }
  void setModeAdd() { this->setMode( FCurveItem::ADD ); }
  void setModeRemove() { this->setMode( FCurveItem::REMOVE ); }
  inline void veXEditFinished() { this->veEditFinished( true ); }
  inline void veYEditFinished() { this->veEditFinished( false ); }

signals:
  void interactionBegin();
  void interactionEnd();
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
