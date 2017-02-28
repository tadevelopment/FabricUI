//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_DFG_DFGERRORSWIDGET_H
#define FABRICUI_DFG_DFGERRORSWIDGET_H

#include <FabricUI/DFG/DFGBindingNotifier.h>
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/StrRef.h>
#include <QIcon>
#include <QWidget>
#include <FabricUI/Actions/BaseAction.h>

class QTableWidget;

namespace FTL {
class JSONArray;
} // namespace FTL

namespace FabricUI {
namespace DFG {

class DFGController;

class DFGErrorsWidget : public QWidget
{
  Q_OBJECT

public:

  DFGErrorsWidget(
    DFGController *dfgController,
    QWidget *parent = 0
    );
  ~DFGErrorsWidget();

  void focusNone();
  void focusBinding();
  void focusExec();

  bool haveErrors();

signals:
  
  void execSelected(
    FTL::CStrRef execPath,
    int line,
    int column
    );
  
  void nodeSelected(
    FTL::CStrRef execPath,
    FTL::CStrRef nodeName,
    int line,
    int column
    );

public slots:
  
  void onErrorsMayHaveChanged();
  void onCustomContextMenuRequested( QPoint const &pos );
  void onDismissSelected();
  void onLoadDiagInserted( unsigned diagIndex );
  void onLoadDiagRemoved( unsigned diagIndex );
  void onCopySelected();

private slots:

  void visitRow( int row, int col );

private:

  enum Focus
  {
    Focus_None,
    Focus_Binding,
    Focus_Exec
  };

  DFGController *m_dfgController;
  Focus m_focus;
  FTL::OwnedPtr<FTL::JSONArray> m_errors;
  QTableWidget *m_tableWidget;
  QIcon m_errorIcon;
  QIcon m_warningIcon;
};

class BaseDFGErrorWidgetAction : public Actions::BaseAction
{
  Q_OBJECT

public:

  BaseDFGErrorWidgetAction(
    DFGErrorsWidget *dfgErrorsWidget,
    QObject *parent,
    const QString &name, 
    const QString &text = "", 
    QKeySequence shortcut = QKeySequence(),
    Qt::ShortcutContext context = Qt::ApplicationShortcut)
    : Actions::BaseAction( 
      parent
      , name 
      , text 
      , shortcut 
      , context)
    , m_dfgErrorsWidget( dfgErrorsWidget )
  {
  }

protected:

  DFGErrorsWidget *m_dfgErrorsWidget;
};

class DismissSelectionAction : public BaseDFGErrorWidgetAction
{
  Q_OBJECT

public:

  DismissSelectionAction(
    DFGErrorsWidget *dfgErrorsWidget,
    QObject *parent)
    : BaseDFGErrorWidgetAction( 
      dfgErrorsWidget
      , parent
      , "BaseDFGErrorWidget::DismissSelectionAction" 
      , "Dismiss Selected")
  {
  }

  private:

    virtual void onTriggered()
    {
      m_dfgErrorsWidget->onDismissSelected();
    }
};

class CopySelectionAction : public BaseDFGErrorWidgetAction
{
  Q_OBJECT

public:

  CopySelectionAction(
    DFGErrorsWidget *dfgErrorsWidget,
    QObject *parent)
    : BaseDFGErrorWidgetAction( 
      dfgErrorsWidget
      , parent
      , "BaseDFGErrorWidget::CopySelectionAction" 
      , "Copy Selected" 
      , QKeySequence(Qt::CTRL + Qt::Key_C) 
      , Qt::WidgetWithChildrenShortcut)
  {
  }

private:

  virtual void onTriggered()
  {
    m_dfgErrorsWidget->onCopySelected();
  }

};


} // namespace DFG
} // namespace FabricUI

#endif // FABRICUI_DFG_DFGERRORSWIDGET_H
