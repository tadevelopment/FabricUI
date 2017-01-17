// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGKLEditorWidget__
#define __UI_DFG_DFGKLEditorWidget__

#include <QFrame>
#include <QPlainTextEdit>
#include <FabricUI/KLEditor/KLEditorWidget.h>
#include "DFGConfig.h"
#include "DFGController.h"


namespace FabricUI
{

  namespace DFG
  {

    class DFGExecHeaderWidget;
    class DFGPEWidget_Exec;

    class DFGKLEditorWidget : public QFrame
    {
      Q_OBJECT

    public:

      DFGKLEditorWidget(
        DFGWidget * dfgWidget,
        DFGExecHeaderWidget *dfgExecHeaderWidget,
        DFGController * controller,
        FabricServices::ASTWrapper::KLASTManager * manager,
        const DFGConfig & config = DFGConfig()
        );
      virtual ~DFGKLEditorWidget();

      bool hasUnsavedChanges() const { return m_unsavedChanges; }
      KLEditor::KLEditorWidget * klEditor() { return m_klEditor; }

      virtual void closeEvent(QCloseEvent * event);

    public slots:

      void onExecChanged();
      void onExecPortsChanged();
      void save();
      void reload();
      void onNewUnsavedChanges();
      void onExecSplitChanged();

    signals:

      void execChanged();

    protected:

      FTL::StrRef getExecPath()
        { return m_controller->getExecPath(); }
      FabricCore::DFGExec &getExec()
        { return m_controller->getExec(); }

      void updateDiags( bool saving = false );

    private:

      DFGController * m_controller;
      DFGPEWidget_Exec *m_dfgPEExecWidget;
      KLEditor::KLEditorWidget * m_klEditor;
      DFGConfig m_config;
      bool m_unsavedChanges;
      bool m_isSettingPorts;
    };

  };

};

#endif // __UI_DFG_DFGKLEditorWidget__
