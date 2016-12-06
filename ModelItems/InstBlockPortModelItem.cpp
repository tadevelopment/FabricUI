//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/InstBlockPortItemMetadata.h>
#include <FabricUI/ModelItems/InstBlockPortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
InstBlockPortModelItem::InstBlockPortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef instBlockPath,
  FTL::StrRef instBlockPortName
  )
  : ItemPortModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    instBlockPath,
    instBlockPortName
    )
{
}

InstBlockPortModelItem::~InstBlockPortModelItem()
{
}

FabricUI::ValueEditor::ItemMetadata *InstBlockPortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new InstBlockPortItemMetadata( this );

  return m_metadata;
}

FTL::CStrRef InstBlockPortModelItem::getName()
{
  return m_portName;
}

bool InstBlockPortModelItem::canRename()
{
  return false;
}

void InstBlockPortModelItem::rename( FTL::CStrRef newInstBlockPortName )
{
  assert( false );
}

void InstBlockPortModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( m_portName == oldName );
  m_portName = newName;
  updatePortPath();
}

} // namespace ModelItems
} // namespace FabricUI
