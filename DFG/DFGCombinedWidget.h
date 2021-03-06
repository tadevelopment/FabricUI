// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef _DFGCOMBINEDWIDGET_H_
#define _DFGCOMBINEDWIDGET_H_

#include <QWidget>
#include <QSplitter>

#include <map>

#include <FabricUI/DFG/DFGUI.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBaseDialog.h>
#include <Commands/CommandStack.h>

#include <FabricUI/ValueEditor/VEEditorOwner.h>

using namespace FabricServices;
using namespace FabricUI;

namespace FabricUI
{
  namespace ValueEditor {
    class VEEditorOwner;
  }

  namespace DFG
  {

    class DFGUICmdHandler;
    
    class DFGCombinedWidget : public QSplitter
    {

      Q_OBJECT
      
    public:
      DFGCombinedWidget(QWidget * parent);

      virtual ~DFGCombinedWidget();

      void init(      
        FabricCore::Client &client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricCore::DFGHost &host,
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        DFGUICmdHandler *cmdHandler,
        bool overTakeBindingNotifications = true,
        DFGConfig config = DFGConfig() );

      static void setLogFunc(DFGController::LogFunc func);

      virtual ASTWrapper::KLASTManager * getManager() { return m_manager; }
      virtual PresetTreeWidget * getTreeWidget() { return m_treeWidget; }
      virtual DFGWidget * getDfgWidget() { return m_dfgWidget; }
      virtual DFGLogWidget * getDfgLogWidget() { return m_dfgLogWidget; }
      virtual QWidget* getDfgValueEditor();

    public slots:
      void log(const char * message) const;
      virtual void onUndo() = 0;
      virtual void onRedo() = 0;
      virtual void onSelectCanvasNodeInDCC() = 0;
      virtual void onImportGraphInDCC() = 0;
      virtual void onExportGraphInDCC() = 0;
      virtual void onGraphSet(FabricUI::GraphView::Graph * graph);
      virtual void onNodeInspectRequested(FabricUI::GraphView::Node * node);
      virtual void onNodeEditRequested(FabricUI::GraphView::Node * node);
      virtual void onAdditionalMenuActionsRequested(QString name, QMenu * menu, bool prefix);

    protected slots:
      void onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData);

    signals:
      void portEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData);

    protected:
      /// Initializes the DFG widget.
      virtual void initDFG();
      /// Initializes the treeView widget.
      virtual void initTreeView();
      /// Initializes the value-editor.
      virtual void initValueEditor();
      /// Initializes the OpenGL.
      virtual void initGL();
      /// Initializes the menus.
      virtual void initMenu();
      /// Initializes the windows docks.
      virtual void initDocks();

      QSplitter *m_hSplitter;
      FabricCore::Client m_client;
      ASTWrapper::KLASTManager *m_manager;
      PresetTreeWidget *m_treeWidget;
      DFGWidget *m_dfgWidget;
      DFGLogWidget *m_dfgLogWidget;
      FabricUI::GraphView::Graph *m_setGraph;
      ValueEditor::VEEditorOwner *m_valueEditor;
      DFG::DFGConfig m_config;
      QMenuBar *m_menuBar;
      FabricCore::DFGBinding m_mainDFGBinding;
    };
  };
};

#endif 
