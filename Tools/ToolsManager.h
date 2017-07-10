//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_TOOLS_MANAGER__
#define __UI_TOOLS_MANAGER__

#include <QObject>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FabricUI/ValueEditor/VEEditorOwner.h>
 
namespace FabricUI {
namespace Tools {

class ToolsManager;

class ToolsManager_NotifProxy : public QObject
{
  public:
    ToolsManager_NotifProxy(
      ToolsManager *dst,
      QObject *parent
      )
      : QObject( parent )
      , m_dst( dst )
      {}

    virtual ~ToolsManager_NotifProxy() 
    {}

  protected:
    ToolsManager *m_dst;
};

class ToolsManager_BindingNotifProxy :
  public ToolsManager_NotifProxy
{
  Q_OBJECT

  public:
    ToolsManager_BindingNotifProxy(
      ToolsManager *dst,
      QObject *parent
      )
      : ToolsManager_NotifProxy( dst, parent )
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

class ToolsManager : public QObject
{
  Q_OBJECT

  public:
    ToolsManager( 
      DFG::DFGWidget * dfgWidget 
      );

    ~ToolsManager();

    virtual void initConnections();

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

    /// Gets the KL tools manager.
    FabricCore::RTVal getKLToolsManager();

    /// Update the value of the tool 
    /// associated to the path `portPath`
    void updateTool(
      FabricCore::RTVal pathValue
      );

  protected slots:
    void onControllerBindingChanged(
      FabricCore::DFGBinding const &binding
      );

    virtual void onSidePanelInspectRequested();

    void onNodeInspectRequested(
      FabricUI::GraphView::Node *node
      );

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

    void onExecPortDefaultValuesChanged(
      FTL::CStrRef portName
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

    virtual void onStructureChanged() {};

    virtual void onGraphSet(
      FabricUI::GraphView::Graph *graph
      );

    FabricUI::DFG::DFGWidget *getDfgWidget();

    FabricUI::DFG::DFGController *getDFGController();

  private:
    void setupConnections(
      FabricUI::DFG::DFGController *dfgController
      );

    void setupConnections(
      FabricCore::DFGExec exec
      );

    DFG::DFGWidget * m_dfgWidget;
    ToolsManager_NotifProxy *m_notifProxy;
    QSharedPointer<DFG::DFGNotifier> m_notifier;
    QSharedPointer<DFG::DFGNotifier> m_subNotifier;
};

} // namespace Tools
} // namespace FabricUI

#endif // __UI_TOOLS_MANAGER__
