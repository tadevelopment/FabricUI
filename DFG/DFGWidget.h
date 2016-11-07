// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGWidget__
#define __UI_DFG_DFGWidget__

#include <QSettings>
#include <QWidget>
#include <QMenuBar>
#include <QProxyStyle>
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
#include <QVBoxLayout>

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
      void deletePort( FabricUI::GraphView::Port *port );
      void editPort( FTL::CStrRef execPortName, bool duplicatePort );

      void movePortsToEnd( bool moveInputs );
      void implodeSelectedNodes( bool displayDialog );

      void createNewGraphNode( QPoint const &globalPos );
      void createNewFunctionNode( QPoint const &globalPos );
      void createNewBackdropNode( QPoint const &globalPos );
      void createNewBlockNode( QPoint const &globalPos );
      void createNewCacheNode( QPoint const &globalPos );
      void createNewVariableNode( QPoint const &globalPos );
      void createNewVariableGetNode( QPoint const &globalPos );
      void createNewVariableSetNode( QPoint const &globalPos );

      void replaceBinding( FabricCore::DFGBinding &binding );
      bool priorExecStackIsEmpty() const
        { return m_priorExecStack.empty(); }

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
      void revealPresetInExplorer(QString);

    public slots:

      void onExecChanged();
      void onExecSplitChanged();
      void onGoUpPressed();
      void onNodeAction(QAction * action);
      void onNodeEditRequested(FabricUI::GraphView::Node *);
      void onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier mod, QString hotkey);
      void onHotkeyReleased(Qt::Key key, Qt::KeyboardModifier mod, QString hotkey);
      void onKeyPressed(QKeyEvent * event);
      void onKeyReleased(QKeyEvent * event);
      void onBubbleEditRequested(FabricUI::GraphView::Node * node);
      void onToggleDimConnections();
      void onTogglePortsCentered();
      void onEditPropertiesForCurrentSelection();
      /// Shows this node in the explorer (if it's a preset)
      void onRevealPresetInExplorer(const char* nodeName);

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

    class DeletePortAction : public QAction
    {
      Q_OBJECT

    public:

      DeletePortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::Port *port,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_port( port )
      {
        setText( "Delete" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->deletePort( m_port );
      }

    private:

      DFGWidget *m_dfgWidget;
      FabricUI::GraphView::Port *m_port;
    };

    class DuplicatePortAction : public QAction
    {
      Q_OBJECT

    public:

      DuplicatePortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::Port *port,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_port( port )
      {
        setText( "Duplicate" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->editPort( m_port->name(), true /* duplicatePort */ );
      }

    private:

      DFGWidget *m_dfgWidget;
      FabricUI::GraphView::Port *m_port;
    };

    class EditPortAction : public QAction
    {
      Q_OBJECT

    public:

      EditPortAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::Port *port,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_port( port )
      {
        setText( "Edit" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->editPort( m_port->name(), false /* duplicatePort */ );
      }

    private:

      DFGWidget *m_dfgWidget;
      FabricUI::GraphView::Port *m_port;
    };

    class NewVariableNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewVariableNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "New variable" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewVariableNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class NewVariableGetNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewVariableGetNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "Read variable (Get)" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewVariableGetNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class NewVariableSetNodeAction : public QAction
    {
      Q_OBJECT

    public:

      NewVariableSetNodeAction(
        DFGWidget *dfgWidget,
        QPoint const &pos,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_pos( pos )
      {
        setText( "Read variable (Set)" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->createNewVariableSetNode( m_pos );
      }

    private:

      DFGWidget *m_dfgWidget;
      QPoint m_pos;
    };

    class MoveInputPortsToEndAction : public QAction
    {
      Q_OBJECT

    public:

      MoveInputPortsToEndAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Move input ports to end" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->movePortsToEnd( true /* moveInputs */ );
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class MoveOutputPortsToEndAction : public QAction
    {
      Q_OBJECT

    public:

      MoveOutputPortsToEndAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Move output ports to end" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->movePortsToEnd( false /* moveInputs */ );
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class ImplodeSelectedNodesAction : public QAction
    {
      Q_OBJECT

    public:

      ImplodeSelectedNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Implode nodes" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
        bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
        m_dfgWidget->implodeSelectedNodes(!isCTRL);
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class SelectAllNodesAction : public QAction
    {
      Q_OBJECT

    public:

      SelectAllNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Select all" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( QKeySequence::SelectAll );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIGraph()->selectAllNodes();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class SidePanelScrollUpAction : public QAction
    {
      Q_OBJECT

    public:

      SidePanelScrollUpAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::SidePanel *sidePanel,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_sidePanel( sidePanel )
      {
        setText( "Scroll up" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_sidePanel->scroll(m_sidePanel->size().height());
      }

    private:

      DFGWidget *m_dfgWidget;
      FabricUI::GraphView::SidePanel *m_sidePanel;
    };

    class SidePanelScrollDownAction : public QAction
    {
      Q_OBJECT

    public:

      SidePanelScrollDownAction(
        DFGWidget *dfgWidget,
        FabricUI::GraphView::SidePanel *sidePanel,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_sidePanel( sidePanel )
      {
        setText( "Scroll down" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_sidePanel->scroll(-m_sidePanel->size().height());
      }

    private:

      DFGWidget *m_dfgWidget;
      FabricUI::GraphView::SidePanel *m_sidePanel;
    };

    class AutoConnectionsAction : public QAction
    {
      Q_OBJECT

    public:

      AutoConnectionsAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Auto connect selected nodes" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_C );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIGraph()->autoConnections();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class RemoveConnectionsAction : public QAction
    {
      Q_OBJECT

    public:

      RemoveConnectionsAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Remove connections to selected node(s)" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_D );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIGraph()->removeConnections();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class CopyNodesAction : public QAction
    {
      Q_OBJECT

    public:

      CopyNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Copy" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( QKeySequence::Copy );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->copy();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class CutNodesAction : public QAction
    {
      Q_OBJECT

    public:

      CutNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Cut" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( QKeySequence::Cut );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->cmdCut();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class PasteNodesAction : public QAction
    {
      Q_OBJECT

    public:

      PasteNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Paste" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( QKeySequence::Paste );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->cmdPaste();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class CollapseLevel1Action : public QAction
    {
      Q_OBJECT

    public:

      CollapseLevel1Action(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Collapse level 1" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_1 );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->setSelectedNodesCollapseState(2);
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class CollapseLevel2Action : public QAction
    {
      Q_OBJECT

    public:

      CollapseLevel2Action(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Collapse level 2" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_2 );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->setSelectedNodesCollapseState(1);
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class CollapseLevel3Action : public QAction
    {
      Q_OBJECT

    public:

      CollapseLevel3Action(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Collapse level 3" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_3 );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->setSelectedNodesCollapseState(0);
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class ResetZoomAction : public QAction
    {
      Q_OBJECT

    public:

      ResetZoomAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Reset zoom" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( QKeySequence(Qt::CTRL + Qt::Key_0) );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->zoomCanvas(1.0);
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class FrameSelectedNodesAction : public QAction
    {
      Q_OBJECT

    public:

      FrameSelectedNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Frame selected" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_F );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->frameSelectedNodes();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class FrameAllNodesAction : public QAction
    {
      Q_OBJECT

    public:

      FrameAllNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Frame all" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_A );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->frameAllNodes();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class RelaxNodesAction : public QAction
    {
      Q_OBJECT

    public:

      RelaxNodesAction(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Relax nodes" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( QKeySequence(Qt::CTRL + Qt::Key_R) );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->relaxNodes();
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class ReloadExtensionsAction : public QAction
    {
      Q_OBJECT

    public:

      ReloadExtensionsAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_node( node )
      {
        setText( "Reload extension(s)" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->getUIController()->reloadExtensionDependencies(m_node->name().c_str());
      }

    private:

      DFGWidget *m_dfgWidget;
      GraphView::Node *m_node;
    };

    class SetNodeCommentAction : public QAction
    {
      Q_OBJECT

    public:

      SetNodeCommentAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_node( node )
      {
        setText( "Set Comment" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        m_dfgWidget->onBubbleEditRequested(m_node);
      }

    private:

      DFGWidget *m_dfgWidget;
      GraphView::Node *m_node;
    };

    class RemoveNodeCommentAction : public QAction
    {
      Q_OBJECT

    public:

      RemoveNodeCommentAction(
        DFGWidget *dfgWidget,
        GraphView::Node *node,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
        , m_node( node )
      {
        setText( "Remove Comment" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
      }

    private slots:

      void onTriggered()
      {
        QString nodeName = QString::fromUtf8( m_node->name().c_str() );
        m_dfgWidget->getUIController()->setNodeCommentExpanded( nodeName, false );
        m_dfgWidget->getUIController()->cmdSetNodeComment( nodeName, QString() );
      }

    private:

      DFGWidget *m_dfgWidget;
      GraphView::Node *m_node;
    };

    class DeleteNodes1Action : public QAction
    {
      Q_OBJECT

    public:

      DeleteNodes1Action(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Delete" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_Delete );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        if (m_dfgWidget->isEditable())
        {
          std::vector<GraphView::Node *> nodes = m_dfgWidget->getUIGraph()->selectedNodes();
          m_dfgWidget->getUIController()->gvcDoRemoveNodes(nodes);
        }
      }

    private:

      DFGWidget *m_dfgWidget;
    };

    class DeleteNodes2Action : public QAction
    {
      Q_OBJECT

    public:

      DeleteNodes2Action(
        DFGWidget *dfgWidget,
        QObject *parent )
        : QAction( parent )
        , m_dfgWidget( dfgWidget )
      {
        setText( "Delete" );
        connect(
          this, SIGNAL(triggered()),
          this, SLOT(onTriggered())
          );
        setShortcut( Qt::Key_Backspace );
        setShortcutContext(Qt::WidgetWithChildrenShortcut);
      }

    private slots:

      void onTriggered()
      {
        if (m_dfgWidget->isEditable())
        {
          std::vector<GraphView::Node *> nodes = m_dfgWidget->getUIGraph()->selectedNodes();
          m_dfgWidget->getUIController()->gvcDoRemoveNodes(nodes);
        }
      }

    private:

      DFGWidget *m_dfgWidget;
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
