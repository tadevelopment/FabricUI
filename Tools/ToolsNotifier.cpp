//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ToolsNotifier.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
#include <FabricUI/DFG/Commands/DFGPathValueResolver.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

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

ToolsNotifierRegistry::ToolsNotifierRegistry( 
  DFGWidget * dfgWidget)
  : m_dfgWidget(dfgWidget)
  , m_notifProxy( NULL )
{
  // FabricCore::DFGExec exec =  m_dfgWidget->getDFGController()->getExec();
  // setupConnections( exec );
}

ToolsNotifierRegistry::~ToolsNotifierRegistry()
{
}

void ToolsNotifierRegistry::initConnections()
{
  std::cout << "ToolsNotifierRegistry::initConnections" << std::endl;
  connect(
    m_dfgWidget->getDFGController(),
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );

  // connect(
  //   m_dfgWidget->getDFGController(), SIGNAL( argsChanged() ),
  //   this, SLOT( onStructureChanged() )
  //   );
 
  // connect(
  //   m_dfgWidget, SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
  //   this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
  //   );

  connect(
    m_dfgWidget, 
    SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
    this, 
    SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) 
    );

  onGraphSet( m_dfgWidget->getUIGraph() );
}

void ToolsNotifierRegistry::onGraphSet( 
  FabricUI::GraphView::Graph * graph)
{
  std::cout << "ToolsNotifierRegistry::onGraphSet" << std::endl;

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

void ToolsNotifierRegistry::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding)
{
  onSidePanelInspectRequested();
}

void ToolsNotifierRegistry::onSidePanelInspectRequested()
{
  DFGController *dfgController =  m_dfgWidget->getDFGController();
  // FabricCore::DFGBinding binding = dfgController->getBinding();
  // std::string path = dfgController->getExecPath();
  
  // std::cout 
  //   << "ToolsNotifierRegistry::onSidePanelInspectRequested " 
  //   << path.c_str()
  //   << std::endl;

  // if ( path.empty() )
  setupConnections(dfgController);
}

void ToolsNotifierRegistry::setupConnections(
  DFGController *dfgController)
{
  std::cout << "ToolsNotifierRegistry::setupConnections " << std::endl;

  if ( m_notifProxy )
  {
    m_notifProxy->setParent( NULL );
    delete m_notifProxy;
    m_notifProxy = NULL;
  }

  m_notifier.clear();
  m_notifier = dfgController->getBindingNotifier();

  ToolsNotifierRegistry_BindingNotifProxy *notifProxy =
    new ToolsNotifierRegistry_BindingNotifProxy( this, this );
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

void ToolsNotifierRegistry::onBindingArgInserted( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef type)
{
  std::cout << "ToolsNotifierRegistry::onBindingArgInserted" << std::endl;
}

void ToolsNotifierRegistry::onBindingArgTypeChanged( 
  unsigned index, 
  FTL::CStrRef name, 
  FTL::CStrRef newType)
{
  std::cout << "ToolsNotifierRegistry::onBindingArgTypeChanged" << std::endl;
}

void ToolsNotifierRegistry::onBindingArgRemoved( 
  unsigned index, 
  FTL::CStrRef name)
{
  std::cout << "ToolsNotifierRegistry::onBindingArgRemoved" << std::endl;
}

void ToolsNotifierRegistry::onBindingArgsReordered( 
  FTL::ArrayRef<unsigned> newOrder)
{
  std::cout << "ToolsNotifierRegistry::onBindingArgsReordered" << std::endl;
}

void ToolsNotifierRegistry::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  std::cout << "ToolsNotifierRegistry::onBindingArgRenamed" << std::endl;
}

void ToolsNotifierRegistry::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  std::cout << "ToolsNotifierRegistry::onBindingArgValueChanged" << std::endl;
  toolValueChanged(name.data());
}

RTVal ToolsNotifierRegistry::getKLToolManager()
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

  FABRIC_CATCH_END("ToolsNotifierRegistry::getKLToolManager");

  return toolRegistry;
}

void ToolsNotifierRegistry::createPathValueTool(
  RTVal pathValue)
{
  getKLToolManager().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    1,
    &pathValue);

  ToolsNotifier *notifier = new ToolsNotifier(
    this,
    pathValue);

  m_registeredNotifiers.append(notifier);

  // Update the tool'value from its pathValue.
  toolValueChanged(pathValue);
}

void ToolsNotifierRegistry::deletePathValueTool(
  RTVal pathValue)
{
  getKLToolManager().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    1,
    &pathValue);

  ToolsNotifier *notifier = new ToolsNotifier(
    this,
    pathValue);

  m_registeredNotifiers.append(notifier);

  // Update the tool'value from its pathValue.
  toolValueChanged(pathValue);
}


void ToolsNotifierRegistry::toolValueChanged(
  QString portPath)
{
  FABRIC_CATCH_BEGIN();

  FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();

  Client client = appStates->getClient();
  RTVal portPathVal = RTVal::ConstructString(client, portPath.toUtf8().constData());
  RTVal pathValue = RTVal::Construct(client, "PathValue", 1, &portPathVal);

  PathValueResolverRegistry *resolverRegistry = PathValueResolverRegistry::getRegistry();
  DFGPathValueResolver *resolver = qobject_cast<DFGPathValueResolver *>(
    resolverRegistry->getResolver(pathValue)
    );

  if(resolver)
  {
    resolver->getValue(pathValue);
    toolValueChanged(pathValue);
  }

  FABRIC_CATCH_END("ToolsNotifierRegistry::toolValueChanged");
}

void ToolsNotifierRegistry::toolValueChanged(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  getKLToolManager().callMethod(
    "",
    "toolValueChanged",
    1,
    &pathValue);

  FABRIC_CATCH_END("ToolsNotifierRegistry::toolValueChanged");
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgValueChanged( index, name );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgInserted(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef type)
{
  m_dst->onBindingArgInserted( index, name, type );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName)
{
  m_dst->onBindingArgRenamed( argIndex, oldArgName, newArgName );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgRemoved(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgRemoved( index, name );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgTypeChanged(
  unsigned index,
  FTL::CStrRef name,
  FTL::CStrRef newType)
{
  m_dst->onBindingArgTypeChanged( index, name, newType );
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgsReordered(
  FTL::ArrayRef<unsigned> newOrder)
{
  m_dst->onBindingArgsReordered( newOrder );
}






ToolsNotifier::ToolsNotifier( 
  ToolsNotifierRegistry *registry,
  RTVal pathValue)
  : m_registry(registry)
{
  PathValueResolverRegistry *resolverRegistry = PathValueResolverRegistry::getRegistry();
  DFGPathValueResolver *resolver = qobject_cast<DFGPathValueResolver *>(
    resolverRegistry->getResolver(pathValue)
    );

  if(resolver)
  {
    QString relPortPath;
    DFGExec exec = resolver->getSubExecAndPortPath(
      pathValue, 
      relPortPath
      );

    FabricCore::String path = exec.getExecPath();
    m_execPath = std::string(path.getCStr(), path.getSize());
    setupConnections(exec);
  }
}

ToolsNotifier::~ToolsNotifier()
{
}

void ToolsNotifier::setupConnections(
  FabricCore::DFGExec exec)
{
  std::cout << "ToolsNotifier::setupConnections 2" << std::endl;

  // FabricCore::DFGBinding binding = getDFGController()->getBinding();
  // FabricCore::DFGExec rootExec = binding.getExec();
  m_notifier.clear();

  m_notifier = DFG::DFGExecNotifier::Create( exec );
  connect(
    m_notifier.data(),
    SIGNAL(nodeRenamed(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodeRenamed(FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodeRemoved(FTL::CStrRef)),
    this,
    SLOT(onExecNodeRemoved(FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>)),
    this,
    SLOT(onExecNodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(nodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onExecNodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockRenamed(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockRemoved(FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockRemoved(FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortInserted(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortInserted(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortRenamed(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortRemoved(FTL::CStrRef, FTL::CStrRef, unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortsReordered(FTL::CStrRef, FTL::CStrRef, FTL::ArrayRef<unsigned>)),
    this,
    SLOT(onInstBlockPortsReordered(FTL::CStrRef, FTL::CStrRef, FTL::ArrayRef<unsigned>))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL(instBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
    this,
    SLOT(onInstBlockPortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(),
    SIGNAL( portsConnected( FTL::CStrRef, FTL::CStrRef ) ),
    this,
    SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
    );
  connect(
    m_notifier.data(),
    SIGNAL( portsDisconnected( FTL::CStrRef, FTL::CStrRef ) ),
    this,
    SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
    );
  connect(
    m_notifier.data(),
    SIGNAL( refVarPathChanged( FTL::CStrRef, FTL::CStrRef ) ),
    this,
    SLOT( onExecRefVarPathChanged( FTL::CStrRef, FTL::CStrRef ) )
    );
}

void ToolsNotifier::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();

  QString portPath = !m_execPath.empty()
    ? QString(m_execPath.c_str()) + "."
    : "";

  portPath += nodeName != ""
    ? QString(nodeName.c_str()) + "." + QString(portName.c_str())
    : portName.c_str();

  m_registry->toolValueChanged(portPath);
 
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortDefaultValuesChanged");
}

void ToolsNotifier::onInstBlockPortDefaultValuesChanged(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onInstBlockPortDefaultValuesChanged" << std::endl;
}

void ToolsNotifier::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  std::cout << "ToolsNotifier::onExecNodePortResolvedTypeChanged" << std::endl;
}

void ToolsNotifier::onInstBlockPortResolvedTypeChanged(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  std::cout << "ToolsNotifier::onInstBlockPortResolvedTypeChanged" << std::endl;
}

void ToolsNotifier::onExecNodePortInserted(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onExecNodePortInserted" << std::endl;
}

void ToolsNotifier::onInstBlockPortInserted(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onInstBlockPortInserted" << std::endl;
}

void ToolsNotifier::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  std::cout << "ToolsNotifier::onExecNodePortRenamed" << std::endl;
}

void ToolsNotifier::onInstBlockPortRenamed(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  std::cout << "ToolsNotifier::onInstBlockPortRenamed" << std::endl;
}

void ToolsNotifier::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onExecNodePortRemoved" << std::endl;
}

void ToolsNotifier::onInstBlockPortRemoved(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onInstBlockPortRemoved" << std::endl;
}

void ToolsNotifier::onInstBlockPortsReordered(
  FTL::CStrRef instName,
  FTL::CStrRef blockName,
  FTL::ArrayRef<unsigned> newOrder)
{
  std::cout << "ToolsNotifier::onInstBlockPortsReordered" << std::endl;
}

void ToolsNotifier::onExecNodePortsReordered(
  FTL::CStrRef nodeName,
  FTL::ArrayRef<unsigned> newOrder)
{
  std::cout << "ToolsNotifier::onExecNodePortsReordered" << std::endl;
}

void ToolsNotifier::onExecPortMetadataChanged(
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value)
{
  std::cout << "ToolsNotifier::onExecPortMetadataChanged" << std::endl;
}

void ToolsNotifier::onExecNodeRemoved(
  FTL::CStrRef nodeName)
{
  std::cout << "ToolsNotifier::onExecNodeRemoved" << std::endl;
}

void ToolsNotifier::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName)
{
  std::cout << "ToolsNotifier::onExecNodeRenamed" << std::endl;
}

void ToolsNotifier::onInstBlockRemoved(
  FTL::CStrRef instName,
  FTL::CStrRef blockName)
{
  std::cout << "ToolsNotifier::onInstBlockRemoved" << std::endl;
}

void ToolsNotifier::onInstBlockRenamed(
  FTL::CStrRef instName,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName)
{
  std::cout << "ToolsNotifier::onInstBlockRenamed" << std::endl;
}

void ToolsNotifier::onExecPortsConnectedOrDisconnected(
  FTL::CStrRef srcPortPath,
  FTL::CStrRef dstPortPath)
{
  std::cout << "ToolsNotifier::onExecPortsConnectedOrDisconnected" << std::endl;
}

void ToolsNotifier::onExecRefVarPathChanged(
  FTL::CStrRef refName,
  FTL::CStrRef newVarPath)
{
  std::cout << "ToolsNotifier::onExecRefVarPathChanged" << std::endl;
}
