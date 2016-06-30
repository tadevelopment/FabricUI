//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/InstBlockModelItem.h>
#include <FabricUI/ModelItems/ItemPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

InstBlockModelItem::InstBlockModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef instName,
  FTL::StrRef instBlockName
  )
 : m_dfgUICmdHandler( dfgUICmdHandler )
 , m_binding( binding )
 , m_execPath( execPath )
 , m_exec( exec )
 , m_instName( instName )
 , m_instBlockName( instBlockName )
{
  assert( !instName.empty() );
  assert( !instBlockName.empty() );

  m_instBlockPath = instName;
  m_instBlockPath += '.';
  m_instBlockPath += instBlockName;
}

InstBlockModelItem::~InstBlockModelItem()
{
}

FTL::CStrRef InstBlockModelItem::getName()
{
  return m_instBlockName;
}

bool InstBlockModelItem::canRename()
{
  return false;
}

void InstBlockModelItem::rename( FTL::CStrRef newName )
{
  assert( false );
}

void InstBlockModelItem::onRenamed(
  FTL::CStrRef oldInstBlockName,
  FTL::CStrRef newInstBlockName
  )
{
  assert( m_instBlockName == oldInstBlockName );
  m_instBlockName = newInstBlockName;

  m_instBlockPath = m_instName;
  m_instBlockPath += '.';
  m_instBlockPath += newInstBlockName;

  // for ( ChildVec::iterator it = m_children.begin();
  //   it != m_children.end(); ++it )
  // {
  //   ItemPortModelItem *nodePortModelItem =
  //     static_cast<ItemPortModelItem *>( *it );
  //   nodePortModelItem->onItemRenamed( oldItemPath, newItemPath );
  // }
}

int InstBlockModelItem::getNumChildren()
{
  return 0;
}

FTL::CStrRef InstBlockModelItem::getChildName( int i )
{
  assert( false );
  return FTL::CStrRef();
}

FabricUI::ValueEditor::BaseModelItem *InstBlockModelItem::createChild( FTL::CStrRef portOrBlockName )
{
  assert( false );
  return NULL;
}

QVariant InstBlockModelItem::getValue()
{
  return QVariant();
}

FabricUI::ValueEditor::ItemMetadata* InstBlockModelItem::getMetadata()
{
  return NULL;
}

void InstBlockModelItem::setMetadataImp( const char* key, const char* value, bool canUndo ) /**/
{
  // TODO: Do We need this?
}

} // namespace ModelItems
} // namespace FabricUI
