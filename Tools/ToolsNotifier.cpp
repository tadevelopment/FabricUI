//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include "ToolsNotifier.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
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

ToolsNotifierRegistry::ToolsNotifierRegistry( 
  DFGWidget *dfgWidget)
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
  FABRIC_CATCH_BEGIN();

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

  FABRIC_CATCH_END("ToolsNotifierRegistry::createPathValueTool");
}

void ToolsNotifierRegistry::deletePathValueTool(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  getKLToolManager().callMethod(
    "",
    "deleteTool",
    1,
    &pathValue);

  ToolsNotifier *notifier = new ToolsNotifier(
    this,
    pathValue);

  QString toolTargetPath = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();

  foreach(ToolsNotifier *notifier, m_registeredNotifiers)
  {
    if(notifier->getToolTargetPath() == toolTargetPath)
    {
      m_registeredNotifiers.removeAll(notifier);
      delete notifier;
      notifier = 0;
      break;
    }
  }

  FABRIC_CATCH_END("ToolsNotifierRegistry::deletePathValueTool");
}

void ToolsNotifierRegistry::toolValueChanged(
  QString portPath)
{
  FABRIC_CATCH_BEGIN();

  FabricApplicationStates* appStates = FabricApplicationStates::GetAppStates();

  QString portPathWithBinding = 
    QString::number(m_dfgWidget->getDFGController()->getBinding().getBindingID()) + "." + portPath;

  Client client = appStates->getClient();
  RTVal portPathVal = RTVal::ConstructString(client, portPathWithBinding.toUtf8().constData());
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

ToolsNotifierRegistry_NotifProxy::ToolsNotifierRegistry_NotifProxy(
  ToolsNotifierRegistry *dst,
  QObject *parent)
  : QObject( parent )
  , m_dst( dst )
{
}

ToolsNotifierRegistry_NotifProxy::~ToolsNotifierRegistry_NotifProxy() 
{
}

ToolsNotifierRegistry_BindingNotifProxy::ToolsNotifierRegistry_BindingNotifProxy(
  ToolsNotifierRegistry *dst,
  QObject *parent)
  : ToolsNotifierRegistry_NotifProxy( dst, parent )
{
}

void ToolsNotifierRegistry_BindingNotifProxy::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name)
{
  m_dst->onBindingArgValueChanged( index, name );
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

ToolsNotifier::ToolsNotifier( 
  ToolsNotifierRegistry *registry,
  RTVal pathValue)
  : m_registry(registry)
{
  FABRIC_CATCH_BEGIN();

  PathValueResolverRegistry *resolverRegistry = PathValueResolverRegistry::getRegistry();
  DFGPathValueResolver *resolver = qobject_cast<DFGPathValueResolver *>(
    resolverRegistry->getResolver(pathValue)
    );

  if(resolver)
  {
    m_toolTargetPath = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    QString relPortPath;
    DFGExec exec = resolver->getSubExecAndPortPath(
      pathValue, 
      relPortPath
      );

    FabricCore::String path = exec.getExecPath();
    m_execPath = QString(std::string(path.getCStr(), path.getSize()).c_str());
    setupConnections(exec);
  }

  FABRIC_CATCH_END("ToolsNotifier::ToolsNotifier");
}

ToolsNotifier::~ToolsNotifier()
{
  m_notifier.clear();
}

QString ToolsNotifier::getToolTargetPath()
{
  return m_toolTargetPath;
}

void ToolsNotifier::setupConnections(
  FabricCore::DFGExec exec)
{
  std::cout << "ToolsNotifier::setupConnections 2" << std::endl;

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
}

void ToolsNotifier::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName)
{
  FABRIC_CATCH_BEGIN();

  QString portPath = !m_execPath.isEmpty()
    ? m_execPath + "."
    : "";

  portPath += nodeName != ""
    ? QString(nodeName.c_str()) + "." + QString(portName.c_str())
    : portName.c_str();

  m_registry->toolValueChanged(portPath);
 
  FABRIC_CATCH_END("ToolsNotifier::onExecNodePortDefaultValuesChanged");
}
 
void ToolsNotifier::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName)
{
  std::cout << "ToolsNotifier::onExecNodePortResolvedTypeChanged" << std::endl;
}
 
void ToolsNotifier::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName)
{
  std::cout << "ToolsNotifier::onExecNodePortRenamed" << std::endl;
}
 
void ToolsNotifier::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName)
{
  std::cout << "ToolsNotifier::onExecNodePortRemoved" << std::endl;
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
