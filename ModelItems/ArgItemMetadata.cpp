//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/ModelItems/ArgItemMetadata.h>

#include <sstream>

using namespace FabricUI::ModelItems;

ArgItemMetadata::ArgItemMetadata( ArgModelItem *argModelItem )
  : m_argModelItem( argModelItem )
{
  std::stringstream bdid; bdid << m_argModelItem->getBinding().getBindingID();
  m_bindingId = bdid.str();

  std::stringstream pp;
  pp << m_argModelItem->getArgName();
  m_portPath = pp.str();
}
