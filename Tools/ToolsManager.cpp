//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ToolsManager.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

ToolsManager::ToolsManager( 
  DFGWidget * dfgWidget)
  : m_dfgWidget(dfgWidget)
  , m_notifProxy( NULL )
{
}

ToolsManager::~ToolsManager()
{
}

void ToolsManager::initConnections()
{
  std::cout << "ToolsManager::initConnections" << std::endl;
  connect(
    getDFGController(),
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );

  connect(
    getDFGController(), SIGNAL( argsChanged() ),
    this, SLOT( onStructureChanged() )
    );
 
  connect(
    getDfgWidget(), SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
    this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
    );

  connect( getDfgWidget(), SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
                    this, SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) );

  onGraphSet( getDfgWidget()->getUIGraph() );
}

DFGWidget* ToolsManager::getDfgWidget()
{
  return m_dfgWidget;
}

DFGController* ToolsManager::getDFGController()
{
  return getDfgWidget()->getDFGController();
}

void ToolsManager::setupConnections(
  DFGController *dfgController)
{
  std::cout << "ToolsManager::setupConnections 1" << std::endl;

  if ( m_notifProxy )
  {
    m_notifProxy->setParent( NULL );
    delete m_notifProxy;
    m_notifProxy = NULL;
  }

  m_notifier.clear();
 
  {
    m_notifier = dfgController->getBindingNotifier();

    ToolsManager_BindingNotifProxy *notifProxy =
      new ToolsManager_BindingNotifProxy( this, this );
    m_notifProxy = notifProxy;

    connect(
      m_notifier.data(),
      SIGNAL( argInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgRemoved( unsigned, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argValueChanged( unsigned, FTL::CStrRef ) ),
      notifProxy,
      SLOT( onBindingArgValueChanged( unsigned, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argsReordered( FTL::ArrayRef<unsigned> ) ),
      notifProxy,
      SLOT( onBindingArgsReordered( FTL::ArrayRef<unsigned> ) )
      );
  }
}

void ToolsManager::setupConnections(
  FabricCore::DFGExec exec)
{
  std::cout << "ToolsManager::setupConnections 2" << std::endl;

  // FabricCore::DFGBinding binding = getDFGController()->getBinding();
  // FabricCore::DFGExec rootExec = binding.getExec();
  m_subNotifier.clear();

  m_subNotifier = DFG::DFGExecNotifier::Create( exec );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodeRenamed(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodeRenamed(FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodeRemoved(FTL::CStrRef)),
    this,
    SLOT(onExecNodeRemoved(FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>)),
    this,
    SLOT(onExecNodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(nodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockRemoved(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockRemoved(FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockPortInserted(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortInserted(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockPortsReordered(FTL::CStrRef, FTL::CStrRef, FTL::ArrayRef<unsigned>)),
    this,
    SLOT(onInstBlockPortsReordered(FTL::CStrRef, FTL::CStrRef, FTL::ArrayRef<unsigned>))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL(instBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_subNotifier.data(),
    SIGNAL( portsConnected( FTL::CStrRef, FTL::CStrRef ) ),
    this,
    SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
    );
  connect(
    m_subNotifier.data(),
    SIGNAL( portsDisconnected( FTL::CStrRef, FTL::CStrRef ) ),
    this,
    SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
    );
  connect(
    m_subNotifier.data(),
    SIGNAL( refVarPathChanged( FTL::CStrRef, FTL::CStrRef ) ),
    this,
    SLOT( onExecRefVarPathChanged( FTL::CStrRef, FTL::CStrRef ) )
    );

  connect(
    m_subNotifier.data(),
    SIGNAL(execPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );

  connect(
    m_subNotifier.data(),
    SIGNAL(execPortDefaultValuesChanged(FTL::CStrRef)),
    this,
    SLOT(onExecPortDefaultValuesChanged(FTL::CStrRef))
    );
}

void ToolsManager::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding)
{
  onSidePanelInspectRequested();
}

inline std::string SplitLast( 
  std::string& path)
{
  size_t split = path.rfind( '.' );
  std::string res = path.substr( split + 1 );
  if (split == std::string::npos)
    path.clear();
  else
    path = path.substr( 0, split );
  
  return res;
}

void ToolsManager::onSidePanelInspectRequested()
{
  DFGController *dfgController = getDFGController();
  FabricCore::DFGBinding binding = dfgController->getBinding();
  std::string path = dfgController->getExecPath();
  
  std::cout 
    << "ToolsManager::onSidePanelInspectRequested " 
    << path.c_str()
    << std::endl;

  if ( path.empty() )
    setupConnections(dfgController);
  
  FabricCore::DFGExec rootExec = binding.getExec();

  FabricCore::DFGExec exec;
  if ( !path.empty() )
    exec = rootExec.getSubExec( path.c_str() );
  else
    exec = rootExec;
 
  setupConnections(exec);
}

void ToolsManager::onNodeInspectRequested(
  FabricUI::GraphView::Node *node)
{
  FabricCore::DFGExec exec = getDFGController()->getExec();
  setupConnections( exec );
}

void ToolsManager::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  std::cout << "ToolsManager::onBindingArgValueChanged" << std::endl;
  onExecNodePortDefaultValuesChanged("", name);
}

void ToolsManager::onExecPortDefaultValuesChanged(
  FTL::CStrRef portName)
{
  std::cout << "ToolsManager::onExecPortDefaultValuesChanged" << std::endl;
  onExecNodePortDefaultValuesChanged("", portName);
}

RTVal ToolsManager::getKLToolsManager()
{
  RTVal toolRegistry;

  FABRIC_CATCH_BEGIN();

  toolRegistry = RTVal::Create(
    FabricApplicationStates::GetAppStates()->getContext(),
    "Tool::AppToolsManager",
    0,
    0);

  toolRegistry = toolRegistry.callMethod(
    "Tool::ToolsManager",
    "getToolsManager",
    0,
    0);

  FABRIC_CATCH_END("CreateToolCommand::getKLToolsManager");

  return toolRegistry;
}

void ToolsManager::updateTool(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  getKLToolsManager().callMethod(
    "",
    "toolValueChanged",
    1,
    &pathValue);

  FABRIC_CATCH_END("ToolsManager::updateTool");
}

void ToolsManager::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();

  // QString portPath = execPath != ""
  //   ? QString(execPath.c_str()) + "."
  //   : "";

  QString portPath = nodeName != ""
    ? QString(nodeName.c_str()) + "." + QString(portName.c_str())
    : portName.c_str();

  Client client = getDFGController()->getClient();
  RTVal portPathVal = RTVal::ConstructString(client, portPath.toUtf8().constData());
  RTVal pathValue = RTVal::Construct(client, "PathValue", 1, &portPathVal);

  PathValueResolverRegistry *resolverRegistry = PathValueResolverRegistry::getRegistry();
  if(resolverRegistry->knownPath(pathValue))
  {
    resolverRegistry->getValue(pathValue);
    updateTool(pathValue);
  }

  FABRIC_CATCH_END("ToolsManager::onExecNodePortDefaultValuesChanged");
}

void ToolsManager::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  std::cout << "ToolsManager::onInstBlockPortDefaultValuesChanged" << std::endl;
}

void ToolsManager::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  std::cout << "ToolsManager::onExecNodePortResolvedTypeChanged" << std::endl;
}

void ToolsManager::onInstBlockPortResolvedTypeChanged(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  std::cout << "ToolsManager::onInstBlockPortResolvedTypeChanged" << std::endl;
}

void ToolsManager::onBindingArgInserted( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef type)
{
  std::cout << "ToolsManager::onBindingArgInserted" << std::endl;
}

void ToolsManager::onBindingArgTypeChanged( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef newType)
{
  std::cout << "ToolsManager::onBindingArgTypeChanged" << std::endl;
}

void ToolsManager::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  std::cout << "ToolsManager::onBindingArgRemoved" << std::endl;
}

void ToolsManager::onBindingArgsReordered( 
  FTL::ArrayRef<unsigned> newOrder)
{
  std::cout << "ToolsManager::onBindingArgsReordered" << std::endl;
}

void ToolsManager::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName)
{
  std::cout << "ToolsManager::onBindingArgRenamed" << std::endl;
}

void ToolsManager::onExecNodePortInserted(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsManager::onExecNodePortInserted" << std::endl;
}

void ToolsManager::onInstBlockPortInserted(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsManager::onInstBlockPortInserted" << std::endl;
}

void ToolsManager::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  std::cout << "ToolsManager::onExecNodePortRenamed" << std::endl;
}

void ToolsManager::onInstBlockPortRenamed(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  std::cout << "ToolsManager::onInstBlockPortRenamed" << std::endl;
}

void ToolsManager::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsManager::onExecNodePortRemoved" << std::endl;
}

void ToolsManager::onInstBlockPortRemoved(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsManager::onInstBlockPortRemoved" << std::endl;
}

void ToolsManager::onInstBlockPortsReordered(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::ArrayRef<unsigned> newOrder)
{
  std::cout << "ToolsManager::onInstBlockPortsReordered" << std::endl;
}

void ToolsManager::onExecNodePortsReordered(
  FTL::CStrRef nodeName,
  FTL::ArrayRef<unsigned> newOrder)
{
  std::cout << "ToolsManager::onExecNodePortsReordered" << std::endl;
}

void ToolsManager::onExecPortMetadataChanged(
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value)
{
  std::cout << "ToolsManager::onExecPortMetadataChanged" << std::endl;
}

void ToolsManager::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  std::cout << "ToolsManager::onExecNodeRemoved" << std::endl;
}

void ToolsManager::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName)
{
  std::cout << "ToolsManager::onExecNodeRenamed" << std::endl;
}

void ToolsManager::onInstBlockRemoved(
  FTL::CStrRef instName,
  FTL::CStrRef blockName)
{
  std::cout << "ToolsManager::onInstBlockRemoved" << std::endl;
}

void ToolsManager::onInstBlockRenamed(
  FTL::CStrRef instName,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName)
{
  std::cout << "ToolsManager::onInstBlockRenamed" << std::endl;
}

void ToolsManager::onExecPortsConnectedOrDisconnected(
  FTL::CStrRef srcPortPath,
  FTL::CStrRef dstPortPath)
{
  std::cout << "ToolsManager::onExecPortsConnectedOrDisconnected" << std::endl;
}

void ToolsManager::onExecRefVarPathChanged(
  FTL::CStrRef refName,
  FTL::CStrRef newVarPath)
{
  std::cout << "ToolsManager::onExecRefVarPathChanged" << std::endl;
}

void ToolsManager::onGraphSet( 
  FabricUI::GraphView::Graph * graph)
{
  std::cout << "ToolsManager::onGraphSet" << std::endl;

  // connect( 
  //   graph, SIGNAL( sidePanelInspectRequested() ),
  //   this, SLOT( onSidePanelInspectRequested() )
  //   );
  // connect(
  //   graph, SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
  //   this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
  //   );

  onSidePanelInspectRequested();
}

void ToolsManager_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgValueChanged( index, name );
}

void ToolsManager_BindingNotifProxy::onBindingArgInserted(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef type)
{
  m_dst->onBindingArgInserted( index, name, type );
}

void ToolsManager_BindingNotifProxy::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName)
{
  m_dst->onBindingArgRenamed( argIndex, oldArgName, newArgName );
}

void ToolsManager_BindingNotifProxy::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgRemoved( index, name );
}

void ToolsManager_BindingNotifProxy::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newType)
{
  m_dst->onBindingArgTypeChanged( index, name, newType );
}

void ToolsManager_BindingNotifProxy::onBindingArgsReordered(
  FTL::ArrayRef<unsigned> newOrder)
{
  m_dst->onBindingArgsReordered( newOrder );
}
