//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/ItemModelItem.h>
#include <FabricUI/ModelItems/ItemPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

ItemModelItem::ItemModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef itemPath
  )
 : m_dfgUICmdHandler( dfgUICmdHandler )
 , m_binding( binding )
 , m_execPath( execPath )
 , m_exec( exec )
 , m_itemPath( itemPath )
{
  assert( !m_itemPath.empty() );
}

ItemModelItem::~ItemModelItem()
{
}

FTL::CStrRef ItemModelItem::getName()
{
  return m_itemPath;
}

bool ItemModelItem::canRename()
{
  return !m_exec.editWouldSplitFromPreset();
}

void ItemModelItem::rename( FTL::CStrRef newName )
{
  m_dfgUICmdHandler->dfgDoEditNode(
    m_binding,
    QString::fromUtf8( m_execPath.data(), m_execPath.size() ),
    m_exec,
    QString::fromUtf8( m_itemPath.data(), m_itemPath.size() ),
    QString::fromUtf8( newName.data(), newName.size() ),
    QString(),
    QString()
    );
}

void ItemModelItem::onRenamed(
  FTL::CStrRef oldItemPath,
  FTL::CStrRef newItemPath
  )
{
  assert( m_itemPath == oldItemPath );

  m_itemPath = newItemPath;

  for ( ChildVec::iterator it = m_children.begin();
    it != m_children.end(); ++it )
  {
    ItemPortModelItem *nodePortModelItem =
      dynamic_cast<ItemPortModelItem *>( *it );
    if(nodePortModelItem)
      nodePortModelItem->onItemRenamed( oldItemPath, newItemPath );
  }
}

int ItemModelItem::getNumChildren()
{
  return m_exec.getItemPortCount( m_itemPath.c_str() );
}

FTL::CStrRef ItemModelItem::getChildName( int i )
{
  return m_exec.getItemPortName( m_itemPath.c_str(), i );
}

void ItemModelItem::setMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  // TODO: Do We need this?
}

} // namespace ModelItems
} // namespace FabricUI
