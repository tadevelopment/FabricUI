//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_TOOLS_NOTIFIER__
#define __UI_TOOLS_NOTIFIER__

#include <QList>
#include <string>
#include <QObject>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FabricUI/ValueEditor/VEEditorOwner.h>
 
namespace FabricUI {
namespace Tools {

class ToolsNotifierRegistry;

class ToolsNotifierRegistry_NotifProxy : public QObject
{
  public:
    ToolsNotifierRegistry_NotifProxy(
      ToolsNotifierRegistry *dst,
      QObject *parent
      )
      : QObject( parent )
      , m_dst( dst )
      {}

    virtual ~ToolsNotifierRegistry_NotifProxy() 
    {}

  protected:
    ToolsNotifierRegistry *m_dst;
};

class ToolsNotifierRegistry_BindingNotifProxy :
  public ToolsNotifierRegistry_NotifProxy
{
  Q_OBJECT

  public:
    ToolsNotifierRegistry_BindingNotifProxy(
      ToolsNotifierRegistry *dst,
      QObject *parent
      )
      : ToolsNotifierRegistry_NotifProxy( dst, parent )
      {}

  public slots:

    void onBindingArgValueChanged(
      unsigned index,
      FTL::CStrRef name
      );

    void onBindingArgInserted(
      unsigned index,
      FTL::CStrRef name,
      FTL::CStrRef type
      );

    void onBindingArgRenamed(
      unsigned argIndex,
      FTL::CStrRef oldArgName,
      FTL::CStrRef newArgName
      );

    void onBindingArgRemoved(
      unsigned index,
      FTL::CStrRef name
      );

    void onBindingArgTypeChanged(
      unsigned index,
      FTL::CStrRef name,
      FTL::CStrRef newType
      );

    void onBindingArgsReordered(
      FTL::ArrayRef<unsigned> newOrder
      );
};

class ToolsNotifier;

class ToolsNotifierRegistry : public QObject
{
  Q_OBJECT

  public:
    ToolsNotifierRegistry( 
      DFG::DFGWidget * dfgWidget 
      );

    ~ToolsNotifierRegistry();

  public slots:
    void onBindingArgValueChanged( 
      unsigned index, 
      FTL::CStrRef name 
      );

    void onBindingArgInserted(
      unsigned index,
      FTL::CStrRef name,
      FTL::CStrRef type
      );

    void onBindingArgRenamed(
      unsigned argIndex,
      FTL::CStrRef oldArgName,
      FTL::CStrRef newArgName
      );

    void onBindingArgRemoved(
      unsigned index,
      FTL::CStrRef name
      );

    void onBindingArgTypeChanged(
      unsigned index,
      FTL::CStrRef name,
      FTL::CStrRef newType
      );

    void onBindingArgsReordered(
      FTL::ArrayRef<unsigned> newOrder
      );

    void initConnections();

    void createPathValueTool(
      FabricCore::RTVal pathValue
      );

    void deletePathValueTool(
      FabricCore::RTVal pathValue
      );

    void toolValueChanged(
      QString portPath
      );

  private slots:
    void onControllerBindingChanged(
      FabricCore::DFGBinding const &binding
      );

    void onSidePanelInspectRequested();

    void onGraphSet(
      FabricUI::GraphView::Graph *graph
      );

  private:
    /// Update the value of the tool 
    /// associated to the path `portPath`
    void toolValueChanged(
      FabricCore::RTVal pathValue
      );

    /// Gets the KL tools manager.
    FabricCore::RTVal getKLToolManager();

    void setupConnections(
      FabricUI::DFG::DFGController *dfgController
      );

    QList<ToolsNotifier *> m_registeredNotifiers;

    DFG::DFGWidget *m_dfgWidget;
    ToolsNotifierRegistry_NotifProxy *m_notifProxy;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
};

class ToolsNotifier : public QObject
{
  Q_OBJECT

  public:
    ToolsNotifier( 
      ToolsNotifierRegistry *registry,
      FabricCore::RTVal pathValue
      );

    ~ToolsNotifier();

  protected slots:
    void onExecNodePortInserted(
      FTL::CStrRef nodeName,
      unsigned portIndex,
      FTL::CStrRef portName
      );

    void onExecNodePortRenamed(
      FTL::CStrRef nodeName,
      unsigned portIndex,
      FTL::CStrRef oldNodePortName,
      FTL::CStrRef newNodePortName
      );

    void onExecNodePortRemoved(
      FTL::CStrRef nodeName,
      unsigned portIndex,
      FTL::CStrRef portName
      );

    void onExecNodePortsReordered(
      FTL::CStrRef nodeName,
      FTL::ArrayRef<unsigned> newOrder
      );

    void onExecPortMetadataChanged(
      FTL::CStrRef portName,
      FTL::CStrRef key,
      FTL::CStrRef value
      );

    void onExecNodeRemoved(
      FTL::CStrRef nodeName
      );

    void onExecNodeRenamed(
      FTL::CStrRef oldNodeName,
      FTL::CStrRef newNodeName
      );

    void onExecPortsConnectedOrDisconnected(
      FTL::CStrRef srcPortPath,
      FTL::CStrRef dstPortPath
      );

    void onExecNodePortDefaultValuesChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef portName
      );

    void onExecNodePortResolvedTypeChanged(
      FTL::CStrRef nodeName,
      FTL::CStrRef portName,
      FTL::CStrRef newResolvedTypeName
      );

    void onExecRefVarPathChanged(
      FTL::CStrRef refName,
      FTL::CStrRef newVarPath
      );

    // InstBlocks
    void onInstBlockRenamed(
      FTL::CStrRef instName,
      FTL::CStrRef oldBlockName,
      FTL::CStrRef newBlockName
      );

    void onInstBlockRemoved(
      FTL::CStrRef instName,
      FTL::CStrRef blockName
      );

    // InstBlockPorts
    void onInstBlockPortInserted(
      FTL::CStrRef instName,
      FTL::CStrRef blockName,
      unsigned portIndex,
      FTL::CStrRef portName
      );

    void onInstBlockPortRenamed(
      FTL::CStrRef instName,
      FTL::CStrRef blockName,
      unsigned portIndex,
      FTL::CStrRef oldPortName,
      FTL::CStrRef newPortName
      );

    void onInstBlockPortRemoved(
      FTL::CStrRef instName,
      FTL::CStrRef blockName,
      unsigned portIndex,
      FTL::CStrRef portName
      );

    void onInstBlockPortDefaultValuesChanged(
      FTL::CStrRef instName,
      FTL::CStrRef blockName,
      FTL::CStrRef portName
      );

    void onInstBlockPortResolvedTypeChanged(
      FTL::CStrRef instName,
      FTL::CStrRef blockName,
      FTL::CStrRef portName,
      FTL::CStrRef newResolveTypeName
      );

    void onInstBlockPortsReordered(
      FTL::CStrRef instName,
      FTL::CStrRef blockName,
      FTL::ArrayRef<unsigned> newOrder
      );

  private:
    void setupConnections(
      FabricCore::DFGExec exec
      );

    std::string m_execPath;
    ToolsNotifierRegistry *m_registry;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_TOOLS_NOTIFIER__
