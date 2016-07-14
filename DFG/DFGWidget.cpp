// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 
#include <assert.h>
#include <FabricCore.h>
#include <FabricUI/DFG/DFGActions.h>
#include <FabricUI/DFG/DFGErrorsWidget.h>
#include <FabricUI/DFG/DFGExecBlockEditorWidget.h>
#include <FabricUI/DFG/DFGGraphViewWidget.h>
#include <FabricUI/DFG/DFGHotkeys.h>
#include <FabricUI/DFG/DFGMetaDataHelpers.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBlockPropertiesDialog.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>
#include <FabricUI/DFG/Dialogs/DFGGetStringDialog.h>
#include <FabricUI/DFG/Dialogs/DFGGetTextDialog.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>
#include <FabricUI/DFG/Dialogs/DFGPickVariableDialog.h>
#include <FabricUI/DFG/Dialogs/DFGSavePresetDialog.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/Util/LoadFabricStyleSheet.h>
#include <FabricUI/Util/UIRange.h>
#include <FabricUI/Util/DocUrl.h>
#include <FTL/FS.h>
#include <Persistence/RTValToJSONEncoder.hpp>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QColorDialog>
#include <QtGui/QCursor>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QSplitter>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

QSettings * DFGWidget::g_settings = NULL;

DFGWidget::DFGWidget(
  QWidget * parent, 
  FabricCore::Client &client,
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  FabricServices::ASTWrapper::KLASTManager * manager,
  DFGUICmdHandler *cmdHandler,
  const DFGConfig & dfgConfig,
  bool overTakeBindingNotifications
  )
  : QWidget( parent )
  , m_errorsWidget( 0 )
  , m_uiGraph( 0 )
  , m_router( 0 )
  , m_manager( manager )
  , m_dfgConfig( dfgConfig )
  , m_isEditable( false )
{
  reloadStyles();

  m_uiController = new DFGController(
    0,
    this,
    client,
    m_manager,
    cmdHandler,
    overTakeBindingNotifications
    );

  QObject::connect(
    m_uiController.get(), SIGNAL( execChanged() ),
    this, SLOT( onExecChanged() )
    );
  QObject::connect(
    m_uiController.get(), SIGNAL( execSplitChanged() ),
    this, SLOT( onExecSplitChanged() )
    );

  m_uiHeader =
    new DFGExecHeaderWidget(
      this,
      m_uiController.get(),
      dfgConfig.graphConfig
      );
  QObject::connect(
    this, SIGNAL( execChanged() ),
    m_uiHeader, SLOT( refresh() )
    );

  m_uiGraphViewWidget = new DFGGraphViewWidget(this, dfgConfig.graphConfig, NULL);
  QObject::connect(
    m_uiGraphViewWidget, SIGNAL(urlDropped(QUrl, bool)),
    this, SIGNAL(urlDropped(QUrl, bool))
    );
  
  m_klEditor =
    new DFGKLEditorWidget(
      this,
      m_uiHeader,
      m_uiController.get(),
      m_manager,
      m_dfgConfig
      );

  m_execBlockEditorWidget =
    new DFGExecBlockEditorWidget(
      this,
      m_uiHeader
      );

  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing( 0 );
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_uiHeader);
  layout->addWidget(m_uiGraphViewWidget);
  layout->addWidget(m_klEditor);
  layout->addWidget(m_execBlockEditorWidget);

  QWidget *widget = new QWidget;
  widget->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding )
    );
  widget->setLayout( layout );

  m_errorsWidget = new DFGErrorsWidget( m_uiController.get() ); 
  connect(
    m_errorsWidget, SIGNAL(execSelected(FTL::CStrRef, int, int)),
    this, SLOT(onExecSelected(FTL::CStrRef, int, int))
    );
  connect(
    m_errorsWidget, SIGNAL(nodeSelected(FTL::CStrRef, FTL::CStrRef, int, int)),
    this, SLOT(onNodeSelected(FTL::CStrRef, FTL::CStrRef, int, int))
    );

  QSplitter *splitter = new QSplitter;
  splitter->setOrientation( Qt::Vertical );
  splitter->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding )
    );
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setChildrenCollapsible(false);
  splitter->addWidget( widget );
  splitter->setStretchFactor( 0, 4 );
  splitter->addWidget( m_errorsWidget );
  splitter->setStretchFactor( 1, 1 );

  layout = new QVBoxLayout;
  layout->setSpacing( 0 );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( splitter );
  setLayout( layout );

  m_tabSearchWidget = new DFGTabSearchWidget(this, m_dfgConfig);
  m_tabSearchWidget->hide();

  QObject::connect(
    m_uiHeader, SIGNAL(goUpPressed()),
    this, SLOT(onGoUpPressed())
    );

  QObject::connect(
    m_uiController.get(), SIGNAL(nodeEditRequested(FabricUI::GraphView::Node *)), 
    this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node *))
    );

  m_uiController->setHostBindingExec( host, binding, execPath, exec );

  QAction *reloadStylesAction = new QAction( "Reload QSS Styles", this );
  reloadStylesAction->setShortcut( QKeySequence( "Ctrl+Shift+R" ) );
  reloadStylesAction->setShortcutContext( Qt::WindowShortcut );
  connect(
    reloadStylesAction, SIGNAL(triggered()),
    this, SLOT(onReloadStyles())
    );
  addAction( reloadStylesAction );
}

DFGWidget::~DFGWidget()
{
  m_errorsWidget->focusNone();

  if ( m_uiController )
    m_uiController->setRouter( 0 );

  if ( m_router )
    delete m_router;
}

GraphView::Graph * DFGWidget::getUIGraph()
{
  return m_uiGraph;
}

DFGKLEditorWidget * DFGWidget::getKLEditor()
{
  return m_klEditor;
}

DFGController * DFGWidget::getUIController()
{
  return m_uiController.get();
}

DFGTabSearchWidget * DFGWidget::getTabSearchWidget()
{
  return m_tabSearchWidget;
}

DFGGraphViewWidget * DFGWidget::getGraphViewWidget()
{
  return m_uiGraphViewWidget;
}

DFGExecHeaderWidget * DFGWidget::getHeaderWidget()
{
  return m_uiHeader;
}

static void CountNodeTypes(
  FabricCore::DFGExec exec,
  FTL::ArrayRef<GraphView::Node *> nodes,
  unsigned &varNodeCount,
  unsigned &getNodeCount,
  unsigned &setNodeCount,
  unsigned &instNodeCount,
  unsigned &userNodeCount,
  unsigned &blockNodeCount
  )
{
  varNodeCount = 0;
  getNodeCount = 0;
  setNodeCount = 0;
  instNodeCount = 0;
  userNodeCount = 0;
  blockNodeCount = 0;
  for(unsigned int i=0;i<nodes.size();i++)
  {
    if (nodes[i]->isBackDropNode())
    {
      userNodeCount++;
      continue;
    }

    char const * nodeName = nodes[i]->name().c_str();
    if ( exec.isExecBlock( nodeName ) )
      ++blockNodeCount;
    else
    {
      FabricCore::DFGNodeType dfgNodeType = exec.getNodeType( nodeName );
      
      if ( dfgNodeType == FabricCore::DFGNodeType_Var)
        varNodeCount++;
      else if ( dfgNodeType == FabricCore::DFGNodeType_Get)
        getNodeCount++;
      else if ( dfgNodeType == FabricCore::DFGNodeType_Set)
        setNodeCount++;
      else if ( dfgNodeType == FabricCore::DFGNodeType_Inst)
        instNodeCount++;
      else
        userNodeCount++;
    }
  }
}

QMenu* DFGWidget::graphContextMenuCallback(FabricUI::GraphView::Graph* graph, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  DFGController *controller =
    static_cast<DFGController *>( graph->controller() );
  if ( !controller )
    return NULL;
  if ( !graphWidget->isEditable() )
    return NULL;

  QMenu* result = new QMenu(NULL);
  result->addAction(
    new NewGraphNodeAction( graphWidget, QCursor::pos(), result )
    );
  result->addAction(
    new NewFunctionNodeAction( graphWidget, QCursor::pos(), result )
    );
  result->addAction(
    new NewBackdropNodeAction( graphWidget, QCursor::pos(), result )
    );

  std::vector<GraphView::Node *> nodes = graph->selectedNodes();
  unsigned varNodeCount;
  unsigned getNodeCount;
  unsigned setNodeCount;
  unsigned instNodeCount;
  unsigned userNodeCount;
  unsigned blockNodeCount;
  CountNodeTypes(
    controller->getExec(),
    nodes,
    varNodeCount,
    getNodeCount,
    setNodeCount,
    instNodeCount,
    userNodeCount,
    blockNodeCount
    );
  bool onlyInstNodes = instNodeCount == nodes.size();
  if ( onlyInstNodes && nodes.size() > 0 )
  {
    result->addSeparator();
    result->addAction(DFG_IMPLODE_NODE);
  }

  result->addSeparator();
  result->addAction(DFG_NEW_VARIABLE);
  result->addAction(DFG_READ_VARIABLE);
  result->addAction(DFG_WRITE_VARIABLE);
  result->addAction(
    new NewCacheNodeAction( graphWidget, QCursor::pos(), result )
    );

  result->addSeparator();
  QAction *newBlockAction =
    new NewBlockNodeAction( graphWidget, QCursor::pos(), result );
  newBlockAction->setEnabled( controller->getExec().allowsBlocks() );
  result->addAction( newBlockAction );

  result->addSeparator();
  QAction * pasteAction = new QAction(DFG_PASTE_PRESET, graphWidget);
  pasteAction->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_V) );
  // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
  pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  result->addAction(pasteAction);

  QAction * selectAllAction = new QAction(DFG_SELECT_ALL_PRESET, graphWidget);
  selectAllAction->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_A) );
  // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
  selectAllAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  result->addAction(selectAllAction);

  result->addSeparator();

  QAction * removeConnectionsAction = new QAction(DFG_DISCONNECT_ALL_PORTS, graphWidget);
  removeConnectionsAction->setShortcut( QKeySequence(Qt::Key_D) );
  // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
  removeConnectionsAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  result->addAction(removeConnectionsAction);

  result->addSeparator();

  QAction * resetZoomAction = new QAction(DFG_RESET_ZOOM, graphWidget);
  resetZoomAction->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_0) );
  // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
  resetZoomAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  result->addAction(resetZoomAction);

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onGraphAction(QAction*)));
  return result;
}

QMenu *DFGWidget::nodeContextMenuCallback(
  FabricUI::GraphView::Node *uiNode,
  void *dfgWidgetVoidPtr
  )
{
  try
  {
    DFGWidget *dfgWidget = static_cast<DFGWidget *>( dfgWidgetVoidPtr );

    FabricCore::DFGExec &exec = dfgWidget->m_uiController->getExec();

    GraphView::Graph * graph = dfgWidget->m_uiGraph;
    if (graph->controller() == NULL)
      return NULL;
    dfgWidget->m_contextNode = uiNode;

    std::vector<GraphView::Node *> nodes =
      dfgWidget->getUIController()->graph()->selectedNodes();
    unsigned varNodeCount;
    unsigned getNodeCount;
    unsigned setNodeCount;
    unsigned instNodeCount;
    unsigned userNodeCount;
    unsigned blockNodeCount;
    CountNodeTypes(
      exec,
      nodes,
      varNodeCount,
      getNodeCount,
      setNodeCount,
      instNodeCount,
      userNodeCount,
      blockNodeCount
      );
    bool onlyInstNodes = instNodeCount == nodes.size();
    bool onlyInstOrBlockNodes =
      ( instNodeCount + blockNodeCount ) == nodes.size();
    bool someVarNodes = varNodeCount > 0;
    bool someGetNodes = getNodeCount > 0;
    bool someSetNodes = setNodeCount > 0;

    char const * nodeName = uiNode->name().c_str();

    QMenu* result = new QMenu(NULL);

    if ( !exec.isExecBlock( nodeName )
      && exec.getNodeType( nodeName ) == FabricCore::DFGNodeType_Inst )
    {
      QString uiDocUrl = exec.getNodeMetadata( nodeName, "uiDocUrl" );
      if ( uiDocUrl.isEmpty() )
      {
        FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
        uiDocUrl = subExec.getMetadata( "uiDocUrl" );
      }
      if ( !uiDocUrl.isEmpty() )
      {
        result->addAction(DFG_OPEN_PRESET_DOC);
        result->addSeparator();
      }
    }

    bool needSeparator = false;
    if ( onlyInstOrBlockNodes )
    {
      result->addAction(DFG_INSPECT_PRESET);
      result->addSeparator();

      GraphView::Node::EditingTargets editingTargets;
      uiNode->collectEditingTargets( editingTargets );
      int lastPriority = -1;
      bool inTopPriority = true;
      for ( size_t i = 0; i < editingTargets.size(); ++i )
      {
        int priority = editingTargets[i].second;
        bool first = lastPriority == -1;
        if ( !first && priority > lastPriority )
        {
          inTopPriority = false;
          result->addSeparator();
        }

        QAction *editAction = NULL;
        if ( editingTargets[i].first->type() == GraphView::QGraphicsItemType_Node )
          editAction = new EditNodeAction(
            dfgWidget,
            static_cast<GraphView::Node *>( editingTargets[i].first ),
            result
            );
        else if ( editingTargets[i].first->type() == GraphView::QGraphicsItemType_InstBlock )
          editAction = new EditInstBlockAction(
            dfgWidget,
            static_cast<GraphView::InstBlock *>( editingTargets[i].first ),
            result
            );
        else assert( false );
        if ( inTopPriority )
          editAction->setText( editAction->text() + " - I or Shift+DoubleClick" );
        result->addAction( editAction );

        lastPriority = priority;
      }

      needSeparator = true;
    }

    bool needAnotherSeparator = false;

    if ( nodes.size() == 1
      && !someVarNodes
      && !someGetNodes
      && !someSetNodes )
    {
      if (needSeparator)
      {
        result->addSeparator();
        needSeparator = false;
      }

      result->addAction(DFG_EDIT_PRESET_PROPERTIES);
      needAnotherSeparator = true;
    }

    if ( dfgWidget->isEditable() )
    {
      if (needSeparator)
      {
        result->addSeparator();
        needSeparator = false;
      }

      result->addAction(DFG_DELETE_PRESET);
      needAnotherSeparator = true;
    }

    if ( needSeparator || needAnotherSeparator )
      result->addSeparator();

    if (!someVarNodes)
    {
      result->addAction(DFG_COPY_PRESET);
      if (dfgWidget->isEditable())
        result->addAction(DFG_CUT_PRESET);
    }
    else
    {
      result->addAction(DFG_SELECT_ALL_PRESET);
      if (dfgWidget->isEditable())
       result->addAction(DFG_PASTE_PRESET);
    }

    if (nodes.size() && dfgWidget->isEditable())
    {
      result->addSeparator();
      result->addAction(DFG_DISCONNECT_ALL_PORTS);
    }

    if ( onlyInstNodes )
    {
      if ( instNodeCount == 1 )
      {
        result->addSeparator();
        if (dfgWidget->isEditable())
          result->addAction(DFG_CREATE_PRESET);
        result->addAction(DFG_EXPORT_GRAPH);
      }

      if ( dfgWidget->isEditable() )
      {
        result->addSeparator();
        result->addAction(DFG_IMPLODE_NODE);

        if (instNodeCount == 1)
        {
          FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
          if (subExec.getType() == FabricCore::DFGExecType_Graph)
          {
            result->addAction(DFG_EXPLODE_NODE);
          }

          if (subExec.getExtDepCount() > 0)
          {
            result->addSeparator();
            result->addAction(DFG_RELOAD_EXTENSION);
          }
        }
      }
    }

    if (nodes.size() == 1 && dfgWidget->isEditable())
    {
      result->addSeparator();
      result->addAction(DFG_SET_COMMENT);
      result->addAction(DFG_REMOVE_COMMENT);
    }

    dfgWidget->connect(result, SIGNAL(triggered(QAction*)), dfgWidget, SLOT(onNodeAction(QAction*)));
    return result;
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
  return NULL;
}

QMenu* DFGWidget::portContextMenuCallback(
  FabricUI::GraphView::Port* port,
  void* userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  if ( !graphWidget->isEditable() )
    return NULL;

  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  if (!graphWidget->getDFGController()->validPresetSplit())
    return NULL;
  graphWidget->m_contextPort = port;
  QMenu* result = new QMenu(NULL);

  QAction *editAction = new QAction("Edit", result);
  editAction->setEnabled( port->allowEdits() );
  result->addAction(editAction);

  QAction *deleteAction = new QAction("Delete", result);
  deleteAction->setEnabled( port->allowEdits() );
  result->addAction(deleteAction);

  try
  {
    FabricCore::DFGExec &exec = graphWidget->m_uiController->getExec();
    if(exec.getExecPortCount() > 1)
    {
      result->addSeparator();

      result->addAction(DFG_MOVE_INPUTS_TO_END);
      result->addAction(DFG_MOVE_OUTPUTS_TO_END);
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onExecPortAction(QAction*)));
  return result;
}

QMenu *DFGWidget::fixedPortContextMenuCallback(
  FabricUI::GraphView::FixedPort *fixedPort,
  void *userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  if ( !graphWidget->isEditable() )
    return NULL;

  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  if (!graphWidget->getDFGController()->validPresetSplit())
    return NULL;

  QMenu *menu = new QMenu( NULL );
  QAction *dummyAction = new QAction( "Port is locked", menu );
  dummyAction->setEnabled( false );
  menu->addAction( dummyAction );
  return menu;
}

QMenu* DFGWidget::sidePanelContextMenuCallback(
  FabricUI::GraphView::SidePanel* panel,
  void* userData
  )
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  if ( !graphWidget->isEditable() )
    return NULL;
  
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextSidePanel = panel;
  QMenu* result = new QMenu(NULL);

  if ( graphWidget->getDFGController()->validPresetSplit() )
  {
    FabricUI::GraphView::PortType portType = panel->portType();
    QAction *createPortAction =
      new CreatePortAction( graphWidget, portType, result );
    if ( portType != FabricUI::GraphView::PortType_Output )
    {
      FabricCore::DFGExec &exec = graphWidget->getDFGController()->getExec();
      if ( exec.isInstBlockExec() )
        createPortAction->setEnabled( false );
    }
    result->addAction( createPortAction );
    result->addSeparator();
  }
  result->addAction(DFG_SCROLL_UP);
  result->addAction(DFG_SCROLL_DOWN);
  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onSidePanelAction(QAction*)));
  return result;
}

void DFGWidget::onGoUpPressed()
{
  std::string nodeName;
  if ( maybePopExec( nodeName ) )
  {
    getUIGraph()->clearSelection();
    if ( !nodeName.empty() )
      if ( GraphView::Node *uiNode = getUIGraph()->node( nodeName ) )
        uiNode->setSelected( true );
  }
}

void DFGWidget::createNewBlockNode( QPoint const &globalPos )
{
  QString text = "block";

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag( Qt::ControlModifier );
  if (!isCTRL)
  {
    DFGGetStringDialog dialog(NULL, "New block", text, m_dfgConfig, true); 
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: block not created (empty name).");
      return; }
  }

  QString blockName =
    m_uiController->cmdAddBlock(
      text,
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( blockName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewCacheNode( QPoint const &globalPos )
{
  m_uiController->cmdAddInstFromPreset(
    "Fabric.Core.Data.Cache",
    m_uiGraphViewWidget->mapToGraph( globalPos )
    );
}

void DFGWidget::createNewGraphNode( QPoint const &globalPos )
{
  QString text = "graph";
  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
  if (!isCTRL)
  {
    DFGGetStringDialog dialog(NULL, "New Empty Graph", text, m_dfgConfig, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: graph not created (empty name).");
      return; }
  }

  QString nodeName =
    m_uiController->cmdAddInstWithEmptyGraph(
      text.toUtf8().constData(),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );

  m_uiGraph->clearSelection();
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
    uiNode->setSelected( true );
}

void DFGWidget::createNewFunctionNode( QPoint const &globalPos )
{
  QString text = "func";

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
  if (!isCTRL)
  {
    DFGGetStringDialog dialog(NULL, "New Empty Function", text, m_dfgConfig, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: function node not created (empty name).");
      return; }
  }

  m_uiController->beginInteraction();

  static const FTL::CStrRef initialCode = FTL_STR("\
dfgEntry {\n\
  // result = a + b;\n\
}\n");
  QString nodeName =
    m_uiController->cmdAddInstWithEmptyFunc(
      text.toUtf8().constData(),
      QString::fromUtf8( initialCode.c_str() ),
      m_uiGraphViewWidget->mapToGraph( globalPos )
      );
  if ( GraphView::Node *uiNode = m_uiGraph->node( nodeName ) )
  {
    maybeEditNode( uiNode );
  }
  m_uiController->endInteraction();
}

void DFGWidget::createNewBackdropNode( QPoint const &globalPos )
{
  QString text = "backdrop";

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);
  if (!isCTRL)
  {
    DFGGetStringDialog dialog(NULL, "New Backdrop", text, m_dfgConfig, false);
    if(dialog.exec() != QDialog::Accepted)
      return;

    text = dialog.text();
    if(text.length() == 0)
    { m_uiController->log("Warning: no backdrop created (empty name).");
      return; }
  }

  m_uiController->cmdAddBackDrop(
    text.toUtf8().constData(),
    m_uiGraphViewWidget->mapToGraph( globalPos )
    );
}

void DFGWidget::onGraphAction(QAction * action)
{
  QPointF mouseOffset(-40, -15);
  QPointF pos = m_uiGraphViewWidget->mapToScene(m_uiGraphViewWidget->mapFromGlobal(QCursor::pos()));
  pos = m_uiGraph->itemGroup()->mapFromScene(pos);
  pos += mouseOffset;

  Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
  bool isCTRL  = keyMod.testFlag(Qt::ControlModifier);

  if(action->text() == DFG_IMPLODE_NODE)
  {
    QString text = "graph";
    if (!isCTRL)
    {
      DFGGetStringDialog dialog(NULL, "Implode Nodes", text, m_dfgConfig, true);
      if(dialog.exec() != QDialog::Accepted)
        return;

      text = dialog.text();
      if(text.length() == 0)
      { m_uiController->log("Warning: nodes not imploded (empty name).");
        return; }
    }

    const std::vector<GraphView::Node*> & nodes =
      m_uiController->graph()->selectedNodes();

    QStringList nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( size_t i = 0; i < nodes.size(); ++i )
    {
      FTL::CStrRef nodeName = nodes[i]->name();
      nodeNames.push_back( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
    }

    QString newNodeName =
      m_uiController->cmdImplodeNodes(
        nodeNames,
        text.toUtf8().constData()
        );

    m_uiGraph->clearSelection();
    if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeName ) )
      uiNode->setSelected( true );
  }
  else if(action->text() == DFG_NEW_VARIABLE)
  {
    DFGController *controller = getUIController();
    FabricCore::Client client = controller->getClient();
    FabricCore::DFGBinding &binding = controller->getBinding();
    FTL::CStrRef execPath = controller->getExecPath();

    DFGNewVariableDialog dialog( this, client, binding, execPath, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString name = dialog.name();
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();

    if (name.isEmpty())
    { controller->log("Warning: no variable created (empty name).");
      return; }
    if (dataType.isEmpty())
    { controller->log("Warning: no variable created (empty type).");
      return; }

    m_uiController->cmdAddVar(
      name,
      dataType,
      extension,
      pos
      );

    pos += QPointF(30, 30);
  }
  else if(action->text() == DFG_READ_VARIABLE ||
    action->text() == DFG_WRITE_VARIABLE)
  {
    DFGController *controller = getUIController();
    FabricCore::Client client = controller->getClient();
    FabricCore::DFGBinding &binding = controller->getBinding();
    FTL::CStrRef execPath = controller->getExecPath();

    DFGPickVariableDialog dialog(NULL, client, binding, execPath, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString name = dialog.name();
    if(name.length() == 0)
      return;

    if(action->text() == DFG_READ_VARIABLE)
    {
      m_uiController->cmdAddGet(
        "get",
        name.toUtf8().constData(),
        QPointF(pos.x(), pos.y())
        );
    }
    else
    {
      m_uiController->cmdAddSet(
        "set",
        name.toUtf8().constData(),
        QPointF(pos.x(), pos.y())
        );
    }
  }
  else if(action->text() == DFG_RESET_ZOOM)
  {
    onResetZoom();
  }
  else if(action->text() == DFG_SELECT_ALL_PRESET)
  {
    onSelectAll();
  }
  else if(action->text() == DFG_DISCONNECT_ALL_PORTS)
  {
    onRemoveConnections();
  }
  else if(action->text() == DFG_PASTE_PRESET)
  {
    onPaste();
  }
}

void DFGWidget::onNodeAction(QAction * action)
{
  if(m_contextNode == NULL)
    return;

  char const * nodeName = m_contextNode->name().c_str();
  if(action->text() == DFG_OPEN_PRESET_DOC)
  {
    FabricCore::DFGExec &exec = m_uiController->getExec();
    QString uiDocUrl = exec.getNodeMetadata( nodeName, "uiDocUrl" );
    if(uiDocUrl.length() == 0 && exec.getNodeType(nodeName) == FabricCore::DFGNodeType_Inst)
    {
      FabricCore::DFGExec subExec = exec.getSubExec( nodeName );
      uiDocUrl = subExec.getMetadata( "uiDocUrl" );
    }
    Util::DocUrl::openUrl(uiDocUrl);
  }
  else if(action->text() == DFG_INSPECT_PRESET)
  {
    emit nodeInspectRequested(m_contextNode);
  }
  else if(action->text() == DFG_DELETE_PRESET)
  {
    m_uiController->gvcDoRemoveNodes(m_contextNode);
  }
  else if(action->text() == DFG_COPY_PRESET)
  {
    onCopy();
  }
  else if(action->text() == DFG_CUT_PRESET)
  {
    onCut();
  }
  else if(action->text() == DFG_EXPORT_GRAPH)
  {
    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst )
      return;

    FabricCore::DFGExec subExec = exec.getSubExec( nodeName );

    // Create a new binding from a copy of the subExec
    FabricCore::DFGHost &host = m_uiController->getHost();
    FabricCore::DFGBinding newBinding = host.createBindingFromJSON(
      subExec.exportJSON().getCString() );
    FabricCore::DFGExec newBindingExec = newBinding.getExec();

    QString title;
    if ( newBindingExec.isPreset() )
    {
      title = newBindingExec.getTitle();
      if ( title.toLower().endsWith(".canvas") )
        title = title.left( title.length() - 7 );
    }
    else title = nodeName;

    FTL::CStrRef uiNodeColor = exec.getNodeMetadata( nodeName, "uiNodeColor" );
    if(!uiNodeColor.empty())
      newBindingExec.setMetadata("uiNodeColor", uiNodeColor.c_str(), true, true);
    FTL::CStrRef uiHeaderColor = exec.getNodeMetadata( nodeName, "uiHeaderColor" );
    if(!uiHeaderColor.empty())
      newBindingExec.setMetadata("uiHeaderColor", uiHeaderColor.c_str(), true, true);
    FTL::CStrRef uiTextColor = exec.getNodeMetadata( nodeName, "uiTextColor" );
    if(!uiTextColor.empty())
      newBindingExec.setMetadata("uiTextColor", uiTextColor.c_str(), true, true);
    FTL::CStrRef uiTooltip = exec.getNodeMetadata( nodeName, "uiTooltip" );
    if(!uiTooltip.empty())
      newBindingExec.setMetadata("uiTooltip", uiTooltip.c_str(), true, true);
    FTL::CStrRef uiDocUrl = exec.getNodeMetadata( nodeName, "uiDocUrl" );
    if(!uiDocUrl.empty())
      newBindingExec.setMetadata("uiDocUrl", uiDocUrl.c_str(), true, true);
    FTL::CStrRef uiAlwaysShowDaisyChainPorts = exec.getNodeMetadata( nodeName, "uiAlwaysShowDaisyChainPorts" );
    if(!uiAlwaysShowDaisyChainPorts.empty())
      newBindingExec.setMetadata("uiAlwaysShowDaisyChainPorts", uiAlwaysShowDaisyChainPorts.c_str(), true, true);

    QString lastPresetFolder = title;
    if(getSettings())
    {
      lastPresetFolder = getSettings()->value("DFGWidget/lastPresetFolder").toString();
      lastPresetFolder += "/" + title;
    }

    QString filter = "DFG Preset (*.canvas)";
    QString filePath = QFileDialog::getSaveFileName(this, DFG_EXPORT_GRAPH, lastPresetFolder, filter, &filter);
    if(filePath.length() == 0)
      return;
    if(filePath.toLower().endsWith(".canvas.canvas"))
      filePath = filePath.left(filePath.length() - 7);

    if(getSettings())
    {
      QDir dir(filePath);
      dir.cdUp();
      getSettings()->setValue( "DFGWidget/lastPresetFolder", dir.path() );
    }

    std::string filePathStr = filePath.toUtf8().constData();

    try
    {
      // copy all defaults
      for(unsigned int i=0;i<newBindingExec.getExecPortCount();i++)
      {
        char const *newBindingExecPortName = newBindingExec.getExecPortName(i);

        std::string pinPath = nodeName;
        pinPath += ".";
        pinPath += newBindingExecPortName;

        FTL::StrRef rType = exec.getNodePortResolvedType(pinPath.c_str());
        if(rType.size() == 0 || rType.find('$') != rType.end())
          continue;
        if(rType.size() == 0 || rType.find('$') != rType.end())
          rType = subExec.getExecPortResolvedType(i);
        if(rType.size() == 0 || rType.find('$') != rType.end())
          rType = subExec.getExecPortTypeSpec(i);
        if(rType.size() == 0 || rType.find('$') != rType.end())
          continue;

        FabricCore::RTVal val =
          exec.getInstPortResolvedDefaultValue(pinPath.c_str(), rType.data());

        if( val.isValid() ) {
          newBindingExec.setPortDefaultValue( newBindingExecPortName, val, false );

          // Reflect port values as binding args
          newBinding.setArgValue( newBindingExecPortName, val, false );
          newBindingExec.setExecPortMetadata( newBindingExecPortName, DFG_METADATA_UIPERSISTVALUE, "true" );
        }
      }

      std::string json = newBinding.exportJSON().getCString();
      FILE * file = fopen(filePathStr.c_str(), "wb");
      if(file)
      {
        fwrite(json.c_str(), json.length(), 1, file);
        fclose(file);
      }
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }
  }
  else if(action->text() == DFG_CREATE_PRESET)
  {
    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.getNodeType( nodeName ) != FabricCore::DFGNodeType_Inst )
      return;
    FabricCore::DFGExec subExec = exec.getSubExec( nodeName );

    try
    {
      FTL::CStrRef defaultPresetName;
      if ( subExec.isPreset() )
        defaultPresetName = subExec.getTitle();
      else
        defaultPresetName = nodeName;
       
      FabricCore::DFGHost &host = m_uiController->getHost();

      DFGSavePresetDialog dialog(
        this,
        m_uiController.get(),
        true,
        defaultPresetName.c_str()
        );
      while ( true )
      {
        if(dialog.exec() != QDialog::Accepted)
          return;

        QString presetName = dialog.name();
        // QString version = dialog.version();
        QString presetDirPath = dialog.location();

        if(presetName.length() == 0 || presetDirPath.length() == 0)
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "You need to provide a valid name and pick a valid location!");
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          continue;
        }

        if(presetDirPath.startsWith("Fabric.") || presetDirPath.startsWith("Variables.") ||
          presetDirPath == "Fabric" || presetDirPath == "Variables")
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "You can't save a preset into a factory path (below Fabric).");
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          continue;
        }

        QString pathname =
          DFGUICmdHandler::NewPresetPathname(
            host,
            presetDirPath,
            presetName
            );

        if ( pathname.isEmpty() )
        {
          QMessageBox msg(
            QMessageBox::Warning,
            "Fabric Error", 
              "The preset directory '"
            + presetDirPath
            + "' does not have an assocaited path and so the preset cannot be saved."
            );
          msg.addButton( "Ok", QMessageBox::AcceptRole );
          msg.exec();
          continue;
        }

        if ( FTL::FSExists( QSTRING_TO_CONST_CHAR_FILE( pathname ) ) )
        {
          QMessageBox msg(
            QMessageBox::Warning,
            "Fabric Warning", 
              "The file "
            + pathname
            + " already exists.\n"
            + "Are you sure to overwrite the file?"
            );
          msg.addButton( "Cancel", QMessageBox::RejectRole );
          msg.addButton( "Ok", QMessageBox::AcceptRole );
          if ( msg.exec() != QDialog::Accepted )
            continue;
        }

        pathname =
          m_uiController->cmdCreatePreset(
            nodeName,
            presetDirPath.toUtf8().constData(),
            presetName.toUtf8().constData()
            );
        if ( pathname.isEmpty() )
        {
          QMessageBox msg(
            QMessageBox::Warning,
            "Fabric Warning", 
              "The file "
            + pathname
            + " cannot be opened for writing."
            );
          msg.addButton( "Ok", QMessageBox::AcceptRole );
          msg.exec();
          continue;
        }

        emit newPresetSaved( pathname );
        // update the preset search paths within the controller
        m_uiController->onVariablesChanged();
        break;
      }
    }
    catch ( FabricCore::Exception e )
    {
      QMessageBox msg(
        QMessageBox::Warning,
        "Fabric Warning", 
        e.getDesc_cstr()
        );
      msg.addButton("Ok", QMessageBox::AcceptRole);
      msg.exec();
      return;
    }
  }
  else if(action->text() == DFG_IMPLODE_NODE)
  {
    DFGGetStringDialog dialog(NULL, "Implode Nodes", "graph", m_dfgConfig, true);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    const std::vector<GraphView::Node*> & nodes =
      m_uiController->graph()->selectedNodes();

    QStringList nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( size_t i = 0; i < nodes.size(); ++i )
    {
      FTL::CStrRef nodeName = nodes[i]->name();
      nodeNames.push_back( QString::fromUtf8( nodeName.data(), nodeName.size() ) );
    }

    QString newNodeName =
      m_uiController->cmdImplodeNodes(
        nodeNames,
        text.toUtf8().constData()
        );

    m_uiGraph->clearSelection();
    if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeName ) )
      uiNode->setSelected( true );
  }
  else if(action->text() == DFG_EXPLODE_NODE)
  {
    QList<QString> newNodeNames =
      m_uiController->cmdExplodeNode( QString::fromUtf8( nodeName ) );

    m_uiGraph->clearSelection();
    for ( int i = 0; i < newNodeNames.size(); ++i )
    {
      if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeNames.at( i ) ) )
        uiNode->setSelected( true );
    }
  }
  else if(action->text() == DFG_RELOAD_EXTENSION)
  {
    m_uiController->reloadExtensionDependencies(nodeName);
  }
  else if(action->text() == DFG_EDIT_PRESET_PROPERTIES)
  {
    onEditPropertiesForCurrentSelection();
  }
  else if(action->text() == DFG_SET_COMMENT)
  {
    onBubbleEditRequested(m_contextNode);
  }
  else if(action->text() == DFG_REMOVE_COMMENT)
  {
    QString nodeName = QString::fromUtf8( m_contextNode->name().c_str() );
    m_uiController->setNodeCommentExpanded( nodeName, false );
    m_uiController->cmdSetNodeComment( nodeName, QString() );
  }
  else if(action->text() == DFG_DISCONNECT_ALL_PORTS)
  {
    onRemoveConnections();
  }

  m_contextNode = NULL;
}

void DFGWidget::editExecPort( FTL::CStrRef execPortName )
{
  try
  {
    QString execPortName_QS =
      QString::fromUtf8( execPortName.data(), execPortName.size() );

    FabricCore::Client &client = m_uiController->getClient();
    FabricCore::DFGExec &exec = m_uiController->getExec();

    DFGEditPortDialog dialog(
      this,
      client,
      false,
      true, //canEditPortType
      m_dfgConfig,
      true
      );

    dialog.setTitle( execPortName_QS );
    dialog.setDataType(exec.getExecPortTypeSpec(execPortName.c_str()));

    FTL::StrRef uiHidden = exec.getExecPortMetadata(execPortName.c_str(), "uiHidden");
    if( uiHidden == "true" )
      dialog.setHidden();

    FTL::StrRef uiOpaque = exec.getExecPortMetadata(execPortName.c_str(), "uiOpaque");
    if( uiOpaque == "true" )
      dialog.setOpaque();

    FTL::StrRef uiPersistValue = exec.getExecPortMetadata(execPortName.c_str(), DFG_METADATA_UIPERSISTVALUE);
    dialog.setPersistValue( uiPersistValue == "true" );

    bool expandMetadataSection = false; // [FE-6068]

    FTL::StrRef uiRange = exec.getExecPortMetadata(execPortName.c_str(), "uiRange");
    double softMinimum = 0.0;
    double softMaximum = 0.0;
    if(FabricUI::DecodeUIRange(uiRange, softMinimum, softMaximum))
    {
      dialog.setHasSoftRange(true);
      dialog.setSoftRangeMin(softMinimum);
      dialog.setSoftRangeMax(softMaximum);
      expandMetadataSection = true;
    }

    FTL::StrRef uiHardRange = exec.getExecPortMetadata(execPortName.c_str(), "uiHardRange");
    double hardMinimum = 0.0;
    double hardMaximum = 0.0;
    if(FabricUI::DecodeUIRange(uiHardRange, hardMinimum, hardMaximum))
    {
      dialog.setHasHardRange(true);
      dialog.setHardRangeMin(hardMinimum);
      dialog.setHardRangeMax(hardMaximum);
      expandMetadataSection = true;
    }

    FTL::StrRef uiCombo = exec.getExecPortMetadata(execPortName.c_str(), "uiCombo");
    std::string uiComboStr;
    if(uiCombo.size() > 0)
      uiComboStr = uiCombo.data();
    if(uiComboStr.size() > 0)
    {
      if(uiComboStr[0] == '(')
        uiComboStr = uiComboStr.substr(1);
      if(uiComboStr[uiComboStr.size()-1] == ')')
        uiComboStr = uiComboStr.substr(0, uiComboStr.size()-1);
      QStringList parts = QString(uiComboStr.c_str()).split(',');
      dialog.setHasCombo(true);
      dialog.setComboValues(parts);
      expandMetadataSection = true;
    }

    dialog.setSectionCollapsed("metadata", !expandMetadataSection);

    emit portEditDialogCreated(&dialog);

    if(dialog.exec() != QDialog::Accepted)
      return;

    emit portEditDialogInvoked(&dialog, NULL);

    QString newPortName = dialog.title();
    QString typeSpec = dialog.dataType();
    QString extDep = dialog.extension();

    std::string uiMetadata;
    {
      FTL::JSONEnc<> metaDataEnc( uiMetadata );
      FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

      DFGAddMetaDataPair( metaDataObjectEnc, "uiHidden", dialog.hidden() ? "true" : "" );//"" will remove the metadata
      DFGAddMetaDataPair( metaDataObjectEnc, "uiOpaque", dialog.opaque() ? "true" : "" );//"" will remove the metadata
      DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, dialog.persistValue() ? "true" : "" );//"" will remove the metadata

      if(dialog.hasSoftRange())
      {
        QString range = "(" + QString::number(dialog.softRangeMin()) + ", " + QString::number(dialog.softRangeMax()) + ")";
        DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", range.toUtf8().constData() );
      } else
        DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", "" );//"" will remove the metadata

      if(dialog.hasHardRange())
      {
        QString range = "(" + QString::number(dialog.hardRangeMin()) + ", " + QString::number(dialog.hardRangeMax()) + ")";
        DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", range.toUtf8().constData() );
      } else
        DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", "" );//"" will remove the metadata

      if(dialog.hasCombo())
      {
        QStringList combo = dialog.comboValues();
        QString flat = "(";
        for(int i=0;i<combo.length();i++)
        {
          if(i > 0)
            flat += ", ";
          flat += "\"" + combo[i] + "\"";
        }
        flat += ")";
        DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", flat.toUtf8().constData() );
      } else
        DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", "" );//"" will remove the metadata

      emit portEditDialogInvoked(&dialog, &metaDataObjectEnc);
    }

    if ( FTL::StrRef( uiMetadata ) == FTL_STR("{}") )
      uiMetadata.clear();

    m_uiController->cmdEditPort(
      execPortName_QS,
      newPortName,
      exec.getExecPortType( execPortName.c_str() ),
      typeSpec,
      extDep,
      QString::fromUtf8( uiMetadata.c_str() )
      );
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::onExecPortAction(QAction * action)
{
  if(m_contextPort == NULL)
    return;

  char const * portName = m_contextPort->name().c_str();
  if(action->text() == "Delete")
  {
    m_uiController->cmdRemovePort( portName );
  }
  else if(action->text() == "Edit")
  {
    editExecPort( portName );
  }
  else if ( action->text() == DFG_MOVE_INPUTS_TO_END
    || action->text() == DFG_MOVE_OUTPUTS_TO_END )
  {
    try
    {
      FabricCore::DFGBinding &binding = m_uiController->getBinding();
      QString execPath = m_uiController->getExecPath_QS();
      FabricCore::DFGExec &exec = m_uiController->getExec();

      // create an index list with the inputs first
      QList<int> inputsFirst;
      QList<int> outputsFirst;
      for(unsigned i=0;i<exec.getExecPortCount();i++)
      {
        if(exec.getExecPortType(i) == FEC_DFGPortType_IO)
        {
          inputsFirst.append(i);
          outputsFirst.append(i);
        }
      }
      for(unsigned i=0;i<exec.getExecPortCount();i++)
      {
        if(exec.getExecPortType(i) == FEC_DFGPortType_In)
          inputsFirst.append(i);
        else if(exec.getExecPortType(i) == FEC_DFGPortType_Out)
          outputsFirst.append(i);
      }
      for(unsigned i=0;i<exec.getExecPortCount();i++)
      {
        if(exec.getExecPortType(i) == FEC_DFGPortType_In)
          outputsFirst.append(i);
        else if(exec.getExecPortType(i) == FEC_DFGPortType_Out)
          inputsFirst.append(i);
      }

      QList<int> indices;

      if(action->text() == DFG_MOVE_INPUTS_TO_END)
        indices = outputsFirst;
      else if(action->text() == DFG_MOVE_OUTPUTS_TO_END)
        indices = inputsFirst;

      m_uiController->cmdReorderPorts(
        binding,
        execPath,
        exec,
        QString(), // itemPath
        indices
        );
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }
  }

  m_contextPort = NULL;
}

void DFGWidget::createPort( FabricUI::GraphView::PortType portType )
{
  FabricCore::Client &client = m_uiController->getClient();
  FabricCore::DFGExec &exec = m_uiController->getExec();
  bool showPortType = !exec.isInstBlockExec();

  DFGEditPortDialog dialog(
    this,
    client,
    showPortType,
    true, // canEditPortType
    m_dfgConfig,
    true // setAlphaNum
    );

  if ( showPortType )
  {
    if ( portType == FabricUI::GraphView::PortType_Output )
      dialog.setPortType("In");
    else
      dialog.setPortType("Out");
  }

  emit portEditDialogCreated(&dialog);

  if(dialog.exec() != QDialog::Accepted)
    return;

  std::string metaData;
  {
    FTL::JSONEnc<> metaDataEnc( metaData );
    FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
    if(dialog.hidden())
      DFGAddMetaDataPair( metaDataObjectEnc, "uiHidden", "true" );
    if(dialog.opaque())
      DFGAddMetaDataPair( metaDataObjectEnc, "uiOpaque", "true" );

    if(dialog.persistValue())
      DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );

    if(dialog.hasSoftRange())
    {
      QString range = "(" + QString::number(dialog.softRangeMin()) + ", " + QString::number(dialog.softRangeMax()) + ")";
      DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", range.toUtf8().constData() );
    } else
      DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", "" );

    if(dialog.hasHardRange())
    {
      QString range = "(" + QString::number(dialog.hardRangeMin()) + ", " + QString::number(dialog.hardRangeMax()) + ")";
      DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", range.toUtf8().constData() );
    } else
      DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", "" );

    if(dialog.hasCombo())
    {
      QStringList combo = dialog.comboValues();
      QString flat = "(";
      for(int i=0;i<combo.length();i++)
      {
        if(i > 0)
          flat += ", ";
        flat += "\"" + combo[i] + "\"";
      }
      flat += ")";
      DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", flat.toUtf8().constData() );
    }

    emit portEditDialogInvoked(&dialog, &metaDataObjectEnc);
  }

  QString title = dialog.title();

  QString dataType = dialog.dataType();
  QString extDep = dialog.extension();

  if ( !title.isEmpty() )
  {
    FabricCore::DFGPortType portType = FabricCore::DFGPortType_In;
    if ( showPortType )
    {
      if ( dialog.portType() == "Out" )
        portType = FabricCore::DFGPortType_Out;
      else if ( dialog.portType() == "IO" )
        portType = FabricCore::DFGPortType_IO;
    }

    if ( metaData == "{}" )
      metaData = "";

    m_uiController->cmdAddPort(
      title,
      portType,
      dataType,
      QString(), // portToConnect
      extDep,
      QString::fromUtf8( metaData.c_str() )
      );
  }
}  

void DFGWidget::onSidePanelAction(QAction * action)
{
  if(action->text() == DFG_SCROLL_UP)
  {
    m_contextSidePanel->scroll(m_contextSidePanel->size().height());
  }
  else if(action->text() == DFG_SCROLL_DOWN)
  {
    m_contextSidePanel->scroll(-m_contextSidePanel->size().height());
  }
}

void DFGWidget::onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier mod, QString hotkey)
{ 
  if(hotkey == DFGHotkeys::PAN_GRAPH)
  {
    m_uiGraph->mainPanel()->setAlwaysPan(true);
  }
  else if(m_isEditable && (hotkey == DFGHotkeys::DELETE_1 || hotkey == DFGHotkeys::DELETE_2))
  {
    std::vector<GraphView::Node *> nodes = getUIGraph()->selectedNodes();
    getUIController()->gvcDoRemoveNodes(nodes);
  }
  else if(hotkey == DFGHotkeys::FRAME_SELECTED)
  {
    getUIController()->frameSelectedNodes();
  }
  else if(hotkey == DFGHotkeys::FRAME_ALL)
  {
    getUIController()->frameAllNodes();
  }
  else if(m_isEditable && hotkey == DFGHotkeys::TAB_SEARCH)
  {
    if (getUIController()->validPresetSplit())
    {
      QPoint pos = getGraphViewWidget()->lastEventPos();
      pos = getGraphViewWidget()->mapToGlobal(pos);
      getTabSearchWidget()->showForSearch(pos);
    }
  }
  else if(hotkey == DFGHotkeys::SELECT_ALL)
  {
    onSelectAll();
  }
  else if(m_isEditable && hotkey == DFGHotkeys::DISCONNECT_ALL_PORTS)
  {
    onRemoveConnections();
  }
  else if(hotkey == DFGHotkeys::COPY)
  {
    onCopy();
  }
  else if(m_isEditable && hotkey == DFGHotkeys::CUT)
  {
    onCut();
  }
  else if(m_isEditable && hotkey == DFGHotkeys::PASTE)
  {
    onPaste();
  }
  else if(hotkey == DFGHotkeys::GO_UP)
  {
    onGoUpPressed();
  }
  else if ( hotkey == DFGHotkeys::EDIT_PRESET )
  {
    std::vector<GraphView::Node*> const &nodes =
      m_uiController->graph()->selectedNodes();
    if ( nodes.size() == 1 )
      onNodeEditRequested( nodes.front() );
  }
  else if(hotkey == DFGHotkeys::EDIT_PRESET_PROPERTIES)
  {
    onEditPropertiesForCurrentSelection();
  }
  else if(hotkey == DFGHotkeys::RELAX_NODES)
  {
    getUIController()->relaxNodes();
  }
  else if(hotkey == DFGHotkeys::RESET_ZOOM)
  {
    onResetZoom();
  }
  else if(hotkey == DFGHotkeys::COLLAPSE_LEVEL_1)
  {
    getUIController()->setSelectedNodeCollapseState(2);
  }
  else if(hotkey == DFGHotkeys::COLLAPSE_LEVEL_2)
  {
    getUIController()->setSelectedNodeCollapseState(1);
  }
  else if(hotkey == DFGHotkeys::COLLAPSE_LEVEL_3)
  {
    getUIController()->setSelectedNodeCollapseState(0);
  }

  FabricCore::FlagUserInteraction();
}

void DFGWidget::onHotkeyReleased(Qt::Key key, Qt::KeyboardModifier mod, QString hotkey)
{
  if(hotkey == DFGHotkeys::PAN_GRAPH)
  {
    m_uiGraph->mainPanel()->setAlwaysPan(false);
  }
}

void DFGWidget::onKeyPressed(QKeyEvent * event)
{
  if(getUIGraph() && !event->isAutoRepeat() && getUIGraph()->pressHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    keyPressEvent(event);  
}

void DFGWidget::onKeyReleased(QKeyEvent * event)
{
  if(getUIGraph() && !event->isAutoRepeat() && getUIGraph()->releaseHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    keyPressEvent(event);  
}

void DFGWidget::onBubbleEditRequested(FabricUI::GraphView::Node * node)
{
  if ( !m_isEditable )
    return;

  QString text;
  bool visible = true;
  bool collapsed = false;

  GraphView::NodeBubble * bubble = node->bubble();
  if ( bubble )
  {
    text = bubble->text();
    visible = bubble->isVisible();
    collapsed = bubble->isCollapsed();
    bubble->show();
    bubble->expand();
  }

  DFGGetTextDialog dialog(NULL, text);
  if ( dialog.exec() == QDialog::Accepted )
  {
    if ( !text.isEmpty() || !dialog.text().isEmpty() )
    {
      m_uiController->cmdSetNodeComment(
        QString::fromUtf8( node->name().c_str() ),
        dialog.text()
        );
      m_uiController->setNodeCommentExpanded(
        QString::fromUtf8( node->name().c_str() ),
        true
        );
    }
    else
    {
      bubble->hide();
    }
  }
  else if ( bubble )
  {
    if ( collapsed )
      bubble->collapse();
    bubble->setVisible( visible );
  }
}

void DFGWidget::onSelectAll()
{
  getUIGraph()->selectAllNodes();
}

void DFGWidget::onRemoveConnections()
{
  getUIGraph()->removeConnections();
}

void DFGWidget::onCopy()
{
  getUIController()->copy();
}

void DFGWidget::onCut()
{
  getUIController()->cmdCut();
}

void DFGWidget::onPaste()
{
  getUIController()->cmdPaste();
}

void DFGWidget::onResetZoom()
{
  getUIController()->zoomCanvas(1.0);
}

void DFGWidget::onToggleDimConnections()
{
  m_uiGraph->config().dimConnectionLines = !m_uiGraph->config().dimConnectionLines;
  std::vector<GraphView::Connection *> connections = m_uiGraph->connections();
  for(size_t i=0;i<connections.size();i++)
    connections[i]->update();
  // [Julien] FE-5264 Sets the dimConnectionLines property to the settings
  if(getSettings()) getSettings()->setValue( "DFGWidget/dimConnectionLines", m_uiGraph->config().dimConnectionLines );
}

void DFGWidget::onTogglePortsCentered()
{
  m_uiGraph->config().portsCentered = !m_uiGraph->config().portsCentered;
  m_uiGraph->sidePanel(GraphView::PortType_Input)->updateItemGroupScroll();  
  m_uiGraph->sidePanel(GraphView::PortType_Output)->updateItemGroupScroll();  
  // [Julien] FE-5264 Sets the onTogglePortsCentered property to the settings
  if(getSettings()) getSettings()->setValue( "DFGWidget/portsCentered", m_uiGraph->config().portsCentered );
}

bool DFGWidget::maybeEditNode(
  FabricUI::GraphView::Node * node
  )
{
  if ( node->isBackDropNode() )
    return false;

  try
  {
    FTL::CStrRef nodeName = node->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.isExecBlock( nodeName.c_str() ) )
      return maybePushExec( nodeName, exec, nodeName );
    else
    {
      switch ( exec.getNodeType( nodeName.c_str() ) )
      {
        case FabricCore::DFGNodeType_Inst:
        {
          FabricCore::DFGExec subExec = exec.getSubExec( nodeName.c_str() );
          return maybePushExec( nodeName, subExec );
        }

        default: break;
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

bool DFGWidget::maybeEditInstBlock(
  FabricUI::GraphView::InstBlock *instBlock
  )
{
  try
  {
    FabricUI::GraphView::Node *node = instBlock->node();
    FTL::CStrRef instName = node->name();
    FTL::CStrRef blockName = instBlock->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    FabricCore::DFGExec subExec =
      exec.getInstBlockExec( instName.c_str(), blockName.c_str() );
    return maybePushExec( node->name(), subExec );
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

bool DFGWidget::checkForUnsaved()
{
  if(m_klEditor->isVisible() && m_klEditor->hasUnsavedChanges())
  {
    QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
      "You haven't saved the code.\nYou are going to lose the changes.\nSure?");

    msg.addButton("Save Now", QMessageBox::AcceptRole);
    msg.addButton("Ok", QMessageBox::NoRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);

    msg.exec();

    QString clicked = msg.clickedButton()->text();
    if(clicked == "Cancel")
      return false;
    else if(clicked == "Save Now")
    {
      m_klEditor->save();
      if(m_klEditor->hasUnsavedChanges())
        return false;
    }
  }

  return true;
}

bool DFGWidget::maybePushExec(
  FTL::StrRef nodeName,
  FabricCore::DFGExec &exec,
  FTL::StrRef execBlockName
  )
{
  try
  {
    if ( !checkForUnsaved() )
      return false;

    m_priorExecStack.push_back(
      PriorExecStackEntry(
        m_uiController->getExec(),
        std::string( nodeName )
        )
      );
    FabricCore::String execPath = exec.getExecPath();
    m_uiController->setExec(
      FTL::StrRef( execPath.getCStr(), execPath.getSize() ),
      exec,
      execBlockName
      );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "Exception: %s\n", e.getDesc_cstr() );
    return false;
  }
  return true;  
}

bool DFGWidget::maybePopExec( std::string &nodeName )
{
  try
  {
    if ( m_priorExecStack.empty() || !checkForUnsaved() )
      return false;

    FabricCore::DFGExec exec = m_priorExecStack.back().first;
    nodeName = m_priorExecStack.back().second;
    m_priorExecStack.pop_back();
    FabricCore::String execPath = exec.getExecPath();
    m_uiController->setExec(
      FTL::StrRef( execPath.getCStr(), execPath.getSize() ),
      exec
      );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "Exception: %s\n", e.getDesc_cstr() );
    return false;
  }
  return true;  
}

void DFGWidget::onEditPropertiesForCurrentSelection()
{
  FabricUI::DFG::DFGController *controller = getUIController();
  if ( controller )
  {
    std::vector<GraphView::Node *> nodes = getUIGraph()->selectedNodes();
    if (nodes.size() != 1)
    {
      if (nodes.size() == 0)  controller->log("cannot open node editor: no node selected.");
      else                    controller->log("cannot open node editor: more than one node selected.");
      return;
    }
    GraphView::Node *node = nodes[0];

    FabricCore::DFGExec &exec = controller->getExec();
    std::string oldNodeName = node->name();

    if ( exec.isExecBlock( oldNodeName.c_str() ) )
    {
      DFG::DFGBlockPropertiesDialog dialog(
        NULL,
        controller,
        oldNodeName.c_str(),
        getConfig(),
        true
        );
      if ( dialog.exec() )
      {
        std::string nodeMetadata;
        {
          FTL::JSONEnc<> metaDataEnc( nodeMetadata );
          FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

          DFGAddMetaDataPair_Color(
            metaDataObjectEnc,
            "uiNodeColor",
            dialog.getNodeColor()
            );

          DFGAddMetaDataPair_Color(
            metaDataObjectEnc,
            "uiTextColor",
            dialog.getTextColor()
            );
          
          // [Julien] FE-5246
          // Add or remove the geader colo node metadata
          QColor headerColor;  
          if ( dialog.getHeaderColor( headerColor ) )
            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiHeaderColor",
              headerColor
              );
          else
            DFGAddMetaDataPair(
              metaDataObjectEnc,
              "uiHeaderColor",
              FTL::StrRef()
              );
        }

        FTL::CStrRef nodeName = node->name();
        controller->cmdEditNode(
          QString::fromUtf8( nodeName.data(), nodeName.size() ),
          dialog.getScriptName(),
          QString::fromUtf8( nodeMetadata.data(), nodeMetadata.size() ),
          QString()
          );  // undoable.
      }
    }
    else
    {    
      FabricCore::DFGNodeType nodeType = exec.getNodeType( oldNodeName.c_str() );
      if (   nodeType == FabricCore::DFGNodeType_Var
          || nodeType == FabricCore::DFGNodeType_Get
          || nodeType == FabricCore::DFGNodeType_Set)
      {
        controller->log("the node editor is not available for variable nodes.");
        return;
      }

      DFG::DFGNodePropertiesDialog dialog(
        NULL,
        controller,
        oldNodeName.c_str(),
        getConfig(),
        true
        );
      if ( dialog.exec() )
      {
        std::string nodeMetadata;
        {
          FTL::JSONEnc<> metaDataEnc( nodeMetadata );
          FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

          if ( nodeType == FabricCore::DFGNodeType_User )
            DFGAddMetaDataPair(
              metaDataObjectEnc,
              "uiTitle",
              dialog.getText().toUtf8().constData()
              );

          if ( nodeType != FabricCore::DFGNodeType_Inst )
          {
            DFGAddMetaDataPair(
              metaDataObjectEnc,
              "uiTooltip",
              dialog.getToolTip().toUtf8().constData()
              );

            DFGAddMetaDataPair(
              metaDataObjectEnc,
              "uiDocUrl",
              dialog.getDocUrl().toUtf8().constData()
              );

            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiNodeColor",
              dialog.getNodeColor()
              );

            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiTextColor",
              dialog.getTextColor()
              );
            
            // [Julien] FE-5246
            // Add or remove the geader colo node metadata
            QColor headerColor;  
            if ( dialog.getHeaderColor( headerColor ) )
              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiHeaderColor",
                headerColor
                );
            else
              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiHeaderColor",
                FTL::StrRef()
                );
          }
        }

        std::string execMetadata;
        if ( nodeType == FabricCore::DFGNodeType_Inst )
        {
          FabricCore::DFGExec subExec = exec.getSubExec( oldNodeName.c_str() );
          if ( !subExec.editWouldSplitFromPreset() )
          {
            FTL::JSONEnc<> metaDataEnc( execMetadata );
            FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );

            DFGAddMetaDataPair(
              metaDataObjectEnc,
              "uiTooltip",
              dialog.getToolTip().toUtf8().constData()
              );

            DFGAddMetaDataPair(
              metaDataObjectEnc,
              "uiDocUrl",
              dialog.getDocUrl().toUtf8().constData()
              );

            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiNodeColor",
              dialog.getNodeColor()
              );

            DFGAddMetaDataPair_Color(
              metaDataObjectEnc,
              "uiTextColor",
              dialog.getTextColor()
              );
            
            // [Julien] FE-5246
            // Add or remove the geader colo node metadata
            QColor headerColor;  
            if ( dialog.getHeaderColor( headerColor ) )
              DFGAddMetaDataPair_Color(
                metaDataObjectEnc,
                "uiHeaderColor",
                headerColor
                );
            else
              DFGAddMetaDataPair(
                metaDataObjectEnc,
                "uiHeaderColor",
                FTL::StrRef()
                );
          }
        }

        FTL::CStrRef nodeName = node->name();
        controller->cmdEditNode(
          QString::fromUtf8( nodeName.data(), nodeName.size() ),
          dialog.getScriptName(),
          QString::fromUtf8( nodeMetadata.data(), nodeMetadata.size() ),
          QString::fromUtf8( execMetadata.data(), execMetadata.size() )
          );  // undoable.
        
        // [Julien] FE-5246
        // Force update the header/nody node color
        onExecChanged();
      }
    }
  }
}

QSettings * DFGWidget::getSettings()
{
  return g_settings;
}

void DFGWidget::setSettings(QSettings * settings)
{
  g_settings = settings;
}

void DFGWidget::refreshTitle( FTL::CStrRef title )
{
  m_uiHeader->refreshTitle( title );
}

void DFGWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  m_uiHeader->refreshExtDeps( extDeps );
}

void DFGWidget::populateMenuBar(QMenuBar * menuBar, bool addFileMenu, bool addDCCMenu)
{
  // [Julien] FE-5244 : Add Save Graph action to the Canvas widget for DCC Integrations
  // Don't add the edit menu if called from DCC
  QMenu *fileMenu = 0;
  if(addFileMenu) {
    fileMenu = menuBar->addMenu(tr("&File"));
    emit additionalMenuActionsRequested("File", fileMenu, true);
    if(fileMenu->actions().count() > 0)
      fileMenu->addSeparator();
  }

  QMenu *editMenu = menuBar->addMenu(tr("&Edit"));
  QMenu *viewMenu = menuBar->addMenu(tr("&View"));

  // emit the prefix menu entry requests
  emit additionalMenuActionsRequested("Edit", editMenu, true);
  emit additionalMenuActionsRequested("View", viewMenu, true);

  // add separators if required
  if(editMenu->actions().count() > 0)
    editMenu->addSeparator();
  if(viewMenu->actions().count() > 0)
    viewMenu->addSeparator();

  // [Fe-6242] DCC menu.
  QMenu *dccMenu = 0;
  if(addDCCMenu) {
    dccMenu = menuBar->addMenu(tr("&DCC"));
    emit additionalMenuActionsRequested("DCC", dccMenu, true);
    if(dccMenu->actions().count() > 0)
      dccMenu->addSeparator();
  }

  // edit menu
  // [Julien]  When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
  // http://doc.qt.io/qt-4.8/qaction.html#shortcutContext-prop
  // http://doc.qt.io/qt-4.8/qt.html#ShortcutContext-enum
  // http://doc.qt.io/qt-4.8/qkeysequence.html
  QAction * selectAction = editMenu->addAction("Select all");
  QObject::connect(selectAction, SIGNAL(triggered()), this, SLOT(onSelectAll()));
  selectAction->setShortcut( QKeySequence::SelectAll );
  selectAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  QAction * cutAction = editMenu->addAction("Cut");
  QObject::connect(cutAction, SIGNAL(triggered()), this, SLOT(onCut()));
  cutAction->setShortcut( QKeySequence::Cut );
  cutAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  QAction * copyAction = editMenu->addAction("Copy");
  QObject::connect(copyAction, SIGNAL(triggered()), this, SLOT(onCopy()));
  copyAction->setShortcut( QKeySequence::Copy );
  copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  QAction * pasteAction = editMenu->addAction("Paste");
  QObject::connect(pasteAction, SIGNAL(triggered()), this, SLOT(onPaste()));
  pasteAction->setShortcut( QKeySequence::Paste );
  pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  // view menu
  QAction * dimLinesAction = viewMenu->addAction("Dim Connections");
  dimLinesAction->setCheckable(true);
  dimLinesAction->setChecked(m_uiGraph->config().dimConnectionLines);
  QObject::connect(dimLinesAction, SIGNAL(triggered()), this, SLOT(onToggleDimConnections()));
  QAction * portsCenteredAction = viewMenu->addAction("Side Ports Centered");
  portsCenteredAction->setCheckable(true);
  portsCenteredAction->setChecked(m_uiGraph->config().portsCentered);
  QObject::connect(portsCenteredAction, SIGNAL(triggered()), this, SLOT(onTogglePortsCentered()));

  // emit the suffix menu entry requests
  emit additionalMenuActionsRequested("Edit", editMenu, false);
  emit additionalMenuActionsRequested("View", viewMenu, false);
  // [Julien] FE-5244 : Add Save Graph action to the Canvas widget for DCC Integrations  // Don't add the edit menu if called from DCC
  if(fileMenu) emit additionalMenuActionsRequested("File", fileMenu, false);
}

void DFGWidget::onExecChanged()
{
  if ( m_router )
  {
    m_uiController->setRouter( 0 );
    delete m_router;
    m_router = 0;
  }

  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.isValid() )
  {
    m_uiGraph = new GraphView::Graph( NULL, m_dfgConfig.graphConfig );
    m_uiGraph->setController(m_uiController.get());
    m_uiController->setGraph(m_uiGraph);
    m_uiGraph->defineHotkey(Qt::Key_Space, Qt::NoModifier, DFGHotkeys::PAN_GRAPH);

    QObject::connect(
      m_uiGraph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
      this, SLOT(onHotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString))
    );  
    QObject::connect(
      m_uiGraph, SIGNAL(hotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString)), 
      this, SLOT(onHotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString))
    );  
    QObject::connect(
      m_uiGraph, SIGNAL(bubbleEditRequested(FabricUI::GraphView::Node*)), 
      this, SLOT(onBubbleEditRequested(FabricUI::GraphView::Node*))
    );

    onExecSplitChanged();

    // [Julien] FE-5264
    // Before initializing the graph, sets the dimConnectionLines and portsCentered properties
    if(getSettings()) 
    {
      m_uiGraph->config().dimConnectionLines = getSettings()->value( "DFGWidget/dimConnectionLines").toBool();
      m_uiGraph->config().portsCentered = getSettings()->value( "DFGWidget/portsCentered").toBool();
    }
    m_uiGraph->initialize();

    m_router =
      static_cast<DFGNotificationRouter *>( m_uiController->createRouter() );
    m_uiController->setRouter(m_router);
  
    m_uiGraph->setGraphContextMenuCallback( &graphContextMenuCallback, this );
    m_uiGraph->setNodeContextMenuCallback( &nodeContextMenuCallback, this );
    m_uiGraph->setPortContextMenuCallback( &portContextMenuCallback, this );
    m_uiGraph->setFixedPortContextMenuCallback( &fixedPortContextMenuCallback, this );
    m_uiGraph->setSidePanelContextMenuCallback( &sidePanelContextMenuCallback, this );

    if ( !m_uiController->getExecBlockName().empty() )
    {
      m_uiGraphViewWidget->hide();
      m_errorsWidget->focusNone();
    }
    else if(exec.getType() == FabricCore::DFGExecType_Graph)
    {
      m_uiGraphViewWidget->show();
      m_uiGraphViewWidget->setFocus();
      m_errorsWidget->focusBinding();
    }
    else if(exec.getType() == FabricCore::DFGExecType_Func)
    {
      m_uiGraphViewWidget->hide();
      m_errorsWidget->focusExec();
    }

    QString filePath = getenv("FABRIC_DIR");
    filePath += "/Resources/PoweredByFabric_black.png";
    m_uiGraph->setupBackgroundOverlay(QPointF(1, -70), filePath);

    emit onGraphSet(m_uiGraph);
  }
  else
  {
    m_uiGraph = NULL;
    m_uiController->setGraph(NULL);
    m_isEditable = false;
  }

  m_uiGraphViewWidget->setGraph(m_uiGraph);

  if ( m_uiGraph )
  {
    try
    {
      m_router->onGraphSet();
    }
    catch(FabricCore::Exception e)
    {
      printf( "%s\n", e.getDesc_cstr() );
    }

    // FE-4277
    emit onGraphSet(m_uiGraph);
  }

  m_uiController->updateNodeErrors();

  emit execChanged();
}

void DFGWidget::reloadStyles()
{
  QString styleSheet = LoadFabricStyleSheet( "DFGWidget.qss" );
  if ( !styleSheet.isEmpty() )
    setStyleSheet( styleSheet );
}

void DFGWidget::onExecSelected(
  FTL::CStrRef execPath,
  int line,
  int column
  )
{
  FabricCore::DFGBinding binding = m_uiController->getBinding();
  FabricCore::DFGExec rootExec = binding.getExec();
  FabricCore::DFGExec exec = rootExec.getSubExec( execPath.c_str() );
  maybePushExec( FTL::StrRef(), exec );
}

void DFGWidget::onNodeSelected(
  FTL::CStrRef execPath,
  FTL::CStrRef nodeName,
  int line,
  int column
  )
{
  FabricCore::DFGBinding binding = m_uiController->getBinding();
  FabricCore::DFGExec rootExec = binding.getExec();
  FabricCore::DFGExec exec = rootExec.getSubExec( execPath.c_str() );
  if ( maybePushExec( FTL::StrRef(), exec ) )
  {
    QApplication::processEvents(); // Let graph view resize etc.
    m_uiController->focusNode( nodeName );
  }
}

void DFGWidget::onExecSplitChanged()
{
  FabricCore::DFGExec &exec = m_uiController->getExec();
  if ( exec.isValid() )
  {
    m_isEditable = !exec.editWouldSplitFromPreset();
    FabricCore::DFGBinding &binding = m_uiController->getBinding();
    FTL::StrRef bindingEditable = binding.getMetadata( "editable" );
    if ( bindingEditable == "false" )
      m_isEditable = false;
    if ( m_uiGraph )
      m_uiGraph->setEditable( m_isEditable );
  }
}

void DFGWidget::replaceBinding(
  FabricCore::DFGBinding &binding
  )
{
  m_priorExecStack.clear();
  FabricCore::DFGExec exec = binding.getExec();
  m_uiController->setBindingExec( binding, FTL::StrRef(), exec );
}

void DFGWidget::onNodeEditRequested(FabricUI::GraphView::Node *node)
{
  try
  {
    FTL::CStrRef nodeName = node->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.isExecBlock( nodeName.c_str() ) )
      maybeEditNode( node );
    else
    {
      switch ( exec.getNodeType( nodeName.c_str() ) )
      {
        case FabricCore::DFGNodeType_Inst:
        {
          if ( node->isHighlighted() )
          {
            maybeEditNode( node );
            return;
          }

          unsigned instBlockCount = node->instBlockCount();
          for ( unsigned instBlockIndex = 0;
            instBlockIndex < instBlockCount; ++instBlockIndex )
          {
            FabricUI::GraphView::InstBlock *instBlock =
              node->instBlockAtIndex( instBlockIndex );
            if ( instBlock->isHighlighted() )
            {
              maybeEditInstBlock( instBlock );
              return;
            }
          }
        }
        break;

        default: break;
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::onReloadStyles()
{
  qDebug() << "Reloading Fabric stylesheets";
  reloadStyles();
  emit stylesReloaded();
}
