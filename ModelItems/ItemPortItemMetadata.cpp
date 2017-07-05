//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//


#include <FabricUI/ModelItems/ItemPortItemMetadata.h>

#include <sstream>

using namespace FabricUI::ModelItems;

ItemPortItemMetadata::ItemPortItemMetadata( ItemPortModelItem *nodePortModelItem )
  : m_nodePortModelItem( nodePortModelItem )
{
  std::stringstream bdid; bdid << m_nodePortModelItem->getBinding().getBindingID();
  m_bindingId = bdid.str();

  std::stringstream pp;
  pp << m_nodePortModelItem->getExec().getExecPath().getCStr() << "." << m_nodePortModelItem->getPortPath();
  m_portPath = pp.str();
}
