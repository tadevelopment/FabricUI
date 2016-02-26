//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "SHVEEditorOwner.h"

using namespace FabricUI;
using namespace SceneHub;
using namespace ModelItems;

SHVEEditorOwner::SHVEEditorOwner(DFG::DFGWidget * dfgWidget)
  : DFG::DFGVEEditorOwner(dfgWidget)
{
}

SHVEEditorOwner::~SHVEEditorOwner()
{
}

void SHVEEditorOwner::initConnections()
{
  DFG::DFGVEEditorOwner::initConnections();
//   connect(
//     getUIController(),
//     SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
//     this,
//     SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
//     );

//   connect(
//     getUIController(), SIGNAL( argsChanged() ),
//     this, SLOT( onStructureChanged() )
//     );
//   connect(  // [FE-6010]
//     this, SIGNAL( modelItemRenamed( BaseModelItem* ) ),
//     this, SLOT( onStructureChanged() )
//     );

//   connect(
//     getDfgWidget(), SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
//     this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
//     );

//   connect( getDfgWidget(), SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
//                     this, SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) );

//   onGraphSet( getDfgWidget()->getUIGraph() );
}
