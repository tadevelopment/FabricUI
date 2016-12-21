// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFontMetrics>
#include <QSplitter>
#include <QMessageBox>

#include <FabricUI/DFG/PortEditor/DFGPEWidget_Exec.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/SidePanel.h>

#include "DFGErrorsWidget.h"
#include "DFGKLEditorWidget.h"
#include "DFGWidget.h"

#include <FTL/AutoSet.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGKLEditorWidget::DFGKLEditorWidget(
  DFGWidget * dfgWidget,
  DFGExecHeaderWidget *dfgExecHeaderWidget,
  DFGController * controller,
  ASTWrapper::KLASTManager * manager,
  const DFGConfig & config
  )
  : QFrame( dfgWidget )
  , m_controller( controller )
  , m_config( config )
  , m_unsavedChanges( false )
  , m_isSettingPorts( false )
{
  setObjectName( "DFGKLEditorWidget" );

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setMinimumSize(QSize(300, 250));

  setFont(config.fixedFont);

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  connect(
    dfgExecHeaderWidget, SIGNAL(reloadPressed()),
    this, SLOT(reload())
    );
  connect(
    dfgExecHeaderWidget, SIGNAL(savePressed()),
    this, SLOT(save())
    );

  QSplitter * splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);
  splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setChildrenCollapsible(false);

  m_dfgPEExecWidget =
    new DFGPEWidget_Exec(
      dfgWidget,
      m_controller->getBinding(),
      m_controller->getExecPath(),
      m_controller->getExec()
      );

  m_klEditor = new KLEditor::KLEditorWidget(splitter, manager, config.klEditorConfig);

  splitter->addWidget(m_dfgPEExecWidget);
  splitter->setStretchFactor(0, 1);
  splitter->addWidget(m_klEditor);
  splitter->setStretchFactor(1, 7);

  layout->addWidget(splitter);

  QObject::connect(
    controller, SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
  QObject::connect(
    controller, SIGNAL(execSplitChanged()),
    this, SLOT(onExecSplitChanged())
    );
  QObject::connect(m_klEditor->sourceCodeWidget(), SIGNAL(newUnsavedChanged()), this, SLOT(onNewUnsavedChanges()));

  updateDiags();
}

DFGKLEditorWidget::~DFGKLEditorWidget()
{
}

void DFGKLEditorWidget::onExecChanged()
{
  if(m_isSettingPorts)
    return;
  
  FabricCore::DFGExec &exec = m_controller->getExec();

  m_dfgPEExecWidget->setExec(
    m_controller->getBinding(),
    m_controller->getExecPath(),
    exec
    );

  bool isEditable = m_controller->validPresetSplit();

  // [FE-5528] instead of disabling the whole widget and
  // its children we disable only certain widgets and
  // make others read-only, so that one can for example
  // inspect the KL code without having to split the preset.
  m_klEditor->sourceCodeWidget()->setReadOnly(!isEditable);

  if ( exec.getType() == FabricCore::DFGExecType_Func
    && m_controller->getExecBlockName().empty() )
  {
    show();      
    m_klEditor->sourceCodeWidget()->setFocus();

    try
    {
      QString code = getExec().getCode();
      m_klEditor->sourceCodeWidget()->setCodeAndExec( code, &exec );
    }
    catch ( FabricCore::Exception e )
    {
      m_controller->logError(e.getDesc_cstr());
    }

    m_unsavedChanges = false;

    updateDiags();

    emit execChanged();
  }
  else
  {
    hide();
  }
}

void DFGKLEditorWidget::onExecPortsChanged()
{
}

void DFGKLEditorWidget::save()
{
  m_controller->cmdSetCode(
    m_klEditor->sourceCodeWidget()->code()
    );

  m_unsavedChanges = false;

  updateDiags( true );
}

void DFGKLEditorWidget::updateDiags( bool saving )
{
  bool haveErrors = false;
  if ( FabricCore::DFGExec exec = m_controller->getExec() )
  {
    DFGWidget *dfgWidget = m_controller->getDFGWidget();
    DFGErrorsWidget *dfgErrorsWidget = dfgWidget->getErrorsWidget();
    dfgErrorsWidget->onErrorsMayHaveChanged();
    haveErrors = dfgErrorsWidget->haveErrors();
  }

  if ( saving && !haveErrors )
    m_controller->log("Save successful.");
}

void DFGKLEditorWidget::reload()
{
  if(m_unsavedChanges)
  {
    QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
      "You have unsaved changes in the source code window, are you sure?");

    msg.addButton("Ok", QMessageBox::AcceptRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);
    msg.exec();

    QString clicked = msg.clickedButton()->text();
    if(clicked == "Cancel")
      return;
  }

  QString code = m_controller->reloadCode();
  if(code.length() > 0)
  {
    try
    {
      FabricCore::DFGExec &exec = m_controller->getExec();
      m_klEditor->sourceCodeWidget()->setCodeAndExec( code, &exec );
    }
    catch(FabricCore::Exception e)
    {
      m_controller->logError(e.getDesc_cstr());
    }

    m_unsavedChanges = false;

    updateDiags();
  }
}

void DFGKLEditorWidget::onNewUnsavedChanges()
{
  m_unsavedChanges = true;
}

void DFGKLEditorWidget::onExecSplitChanged()
{
  if(!isVisible())
    return;

  FabricCore::DFGExec exec = m_controller->getExec();
  if(exec.getType() == FabricCore::DFGExecType_Func)
    onExecChanged();
}

void DFGKLEditorWidget::closeEvent(QCloseEvent * event)
{

}
