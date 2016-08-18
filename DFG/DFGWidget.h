// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGWidget__
#define __UI_DFG_DFGWidget__

#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QMenuBar>
#include <QtGui/QProxyStyle>
#include <Commands/CommandStack.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/DFG/DFGConfig.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>
#include <FabricUI/DFG/DFGGraphViewWidget.h>
#include <FabricUI/DFG/DFGKLEditorWidget.h>
#include <FabricUI/DFG/DFGNotificationRouter.h>
#include <FabricUI/DFG/DFGTabSearchWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBaseDialog.h>
#include <QtGui/QVBoxLayout>

#include <FTL/OwnedPtr.h>
#include <FTL/JSONEnc.h>

namespace FabricUI {
namespace DFG {
  
    class DFGErrorsWidget;
    class DFGExecBlockEditorWidget;
    class DFGExecHeaderWidget;
    class DFGUICmdHandler;

    class DFGWidgetProxyStyle
      : public QProxyStyle
    {
    public:

      DFGWidgetProxyStyle(
        QStyle* style = NULL
        );

      virtual void drawControl(
        ControlElement element,
        const QStyleOption * option,
        QPainter * painter,
        const QWidget * widget
        ) const /*override*/;
    };

    class DFGWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGWidget(
        QWidget * parent, 
        FabricCore::Client &client,
        FabricCore::DFGHost &host,
        FabricCore::DFGBinding &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec &exec,
        FabricServices::ASTWrapper::KLASTManager * manager,
        DFGUICmdHandler *cmdHandler,
        const DFGConfig & dfgConfig,
        bool overTakeBindingNotifications = true
        );
      virtual ~DFGWidget();

      DFGConfig & getConfig() { return m_dfgConfig; }

      DFGController *getDFGController()
        { return m_uiController.get(); }

      GraphView::Graph * getUIGraph();
      DFGKLEditorWidget * getKLEditor();
      DFGController * getUIController();
      DFGTabSearchWidget * getTabSearchWidget();
      DFGGraphViewWidget * getGraphViewWidget();
      DFGExecHeaderWidget * getHeaderWidget();
      DFGErrorsWidget *getErrorsWidget() const
        { return m_errorsWidget; }

      bool isEditable() const { return m_isEditable; }
      static QSettings * getSettings();
      static void setSettings(QSettings * settings);
      
      void refreshTitle( FTL::CStrRef title );
      void refreshExtDeps( FTL::CStrRef extDeps );

      void populateMenuBar(QMenuBar * menuBar, bool addFileMenu = true, bool addDCCMenu = false);

      bool maybeEditNode( FabricUI::GraphView::Node *node );
      bool maybeEditInstBlock( FabricUI::GraphView::InstBlock *instBlock );

      void reloadStyles();

      void createPort( FabricUI::GraphView::PortType portType );
      void createNewGraphNode( QPoint const &pos );
      void createNewFunctionNode( QPoint const &pos );
      void createNewBackdropNode( QPoint const &pos );
      void createNewBlockNode( QPoint const &pos );
      void createNewCacheNode( QPoint const &pos );

      void replaceBinding( FabricCore::DFGBinding &binding );
      bool priorExecStackIsEmpty() const
        { return m_priorExecStack.empty(); }

      void editExecPort( FTL::CStrRef execPortName, bool duplicatePort );

    signals:

      void additionalMenuActionsRequested(QString, QMenu*, bool);
      void execChanged();
      void newPresetSaved(QString presetFilePath);
      void onGraphSet(FabricUI::GraphView::Graph* graph);
      void portEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData);
      void nodeInspectRequested(FabricUI::GraphView::Node *);
      void urlDropped( QUrl url, bool bypassUnsavedChanges );
      void stylesReloaded();
      
    public slots:

      void onExecChanged();
      void onExecSplitChanged();
      void onGoUpPressed();
      void onGraphAction(QAction * action);
      void onNodeAction(QAction * action);
      void onNodeEditRequested(FabricUI::GraphView::Node *);
      void onExecPortAction(QAction * action);
      void onSidePanelAction(QAction * action);
      void onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier mod, QString hotkey);
      void onHotkeyReleased(Qt::Key key, Qt::KeyboardModifier mod, QString hotkey);
      void onKeyPressed(QKeyEvent * event);
      void onKeyReleased(QKeyEvent * event);
      void onBubbleEditRequested(FabricUI::GraphView::Node * node);
      void onSelectAll();
      void onAutoConnections();
      void onRemoveConnections();
      void onCut();
      void onCopy();
      void onPaste();
      void onResetZoom();
      void onToggleDimConnections();
      void onTogglePortsCentered();
      void onEditPropertiesForCurrentSelection();

    protected slots:

      void onReloadStyles();

    private slots:

      void onExecSelected(
        FTL::CStrRef execPath,
        int line,
        int column
        );

      void onNodeSelected(
        FTL::CStrRef execPath,
        FTL::CStrRef nodeName,
        int line,
        int column
        );

    private:

      static QMenu* graphContextMenuCallback(FabricUI::GraphView::Graph* graph, void* userData);
      static QMenu* nodeContextMenuCallback(FabricUI::GraphView::Node* node, void* userData);
      static QMenu* portContextMenuCallback(FabricUI::GraphView::Port* port, void* userData);
      static QMenu* fixedPortContextMenuCallback(FabricUI::GraphView::FixedPort* fixedPort, void* userData);
      static QMenu* sidePanelContextMenuCallback(FabricUI::GraphView::SidePanel* panel, void* userData);

      bool maybePushExec(
        FTL::StrRef nodeName,
        FabricCore::DFGExec &exec,
        FTL::StrRef execBlockName = FTL::StrRef()
        );
      bool maybePopExec( std::string &nodeName );

      bool checkForUnsaved();

      QPoint m_contextPos;
      FabricUI::GraphView::Node * m_contextNode;
      FabricUI::GraphView::Port * m_contextPort;
      FabricUI::GraphView::SidePanel * m_contextSidePanel;

      DFGGraphViewWidget * m_uiGraphViewWidget;
      DFGExecHeaderWidget * m_uiHeader;
      DFGErrorsWidget *m_errorsWidget;
      GraphView::Graph * m_uiGraph;
      FTL::OwnedPtr<DFGController> m_uiController;
      DFGNotificationRouter * m_router;
      DFGKLEditorWidget * m_klEditor;
      DFGExecBlockEditorWidget *m_execBlockEditorWidget;
      DFGTabSearchWidget * m_tabSearchWidget;
      FabricServices::ASTWrapper::KLASTManager * m_manager;
      DFGConfig m_dfgConfig;

      typedef std::pair<FabricCore::DFGExec, std::string> PriorExecStackEntry;
      std::vector<PriorExecStackEntry> m_priorExecStack;

      bool m_isEditable;

      static QSettings * g_settings;
    };

    class CreatePortAction : public QAction
    {
      Q_OBJECT

    public:

      CreatePortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::PortType portType,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_portType( portType )
      {
        setText( "Create port" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createPort( m_portType );
      }

    private:

      DFGWidget *m_dfgWidget;
      FabricUI::GraphView::PortType m_portType;
    };

    class NewBlockNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewBlockNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "New block" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewBlockNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class NewCacheNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewCacheNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "New cache node" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewCacheNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class NewGraphNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewGraphNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "New empty graph" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewGraphNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class NewFunctionNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewFunctionNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "New empty function" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewFunctionNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class NewBackdropNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewBackdropNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "New backdrop" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewBackdropNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class EditNodeAction : public QAction
    {
      Q_OBJECT

    public:

      EditNodeAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent
        )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_node( node )
      {
        setText( "Edit node" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->maybeEditNode( m_node );
      }

    private:

      DFGWidget *m_dfgWidget;
      GraphView::Node *m_node;
    };

    class EditInstBlockAction : public QAction
    {
      Q_OBJECT

    public:

      EditInstBlockAction(
        DFGWidget *dfgWidget,
        GraphView::InstBlock *instBlock,
        QObject *parent
        )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_instBlock( instBlock )
      {
        setText(
            "Edit block '"
          + m_instBlock->name_QS()
          + "'" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->maybeEditInstBlock( m_instBlock );
      }

    private:

      DFGWidget *m_dfgWidget;
      GraphView::InstBlock *m_instBlock;
    };

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_DFGWidget__
