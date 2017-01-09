//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/InstPortModelItem.h>
#include <FabricUI/ModelItems/InstBlockModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

InstModelItem::InstModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName
  )
 : ItemModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName
    )
{
}

InstModelItem::~InstModelItem()
{
}

int InstModelItem::getNumChildren()
{
  return m_exec.getItemPortCount( m_itemPath.c_str() )
    + m_exec.getInstBlockCount( m_itemPath.c_str() );
}

FTL::CStrRef InstModelItem::getChildName( int i )
{
  int itemPortCount = int( m_exec.getItemPortCount( m_itemPath.c_str() ) );
  if ( i < itemPortCount )
    return m_exec.getItemPortName( m_itemPath.c_str(), i );
  else
    return m_exec.getInstBlockName( m_itemPath.c_str(), i - itemPortCount );
}

FabricUI::ValueEditor::BaseModelItem *
InstModelItem::createChild( FTL::StrRef portOrBlockName )
{
  if ( m_exec.isInstBlock(
    m_itemPath.c_str(),
    std::string( portOrBlockName ).c_str()
    ) )
    return pushChild( new InstBlockModelItem(
      m_dfgUICmdHandler,
      m_binding,
      m_execPath,
      m_exec,
      m_itemPath,
      portOrBlockName
      ) );
  else
    return pushChild( new InstPortModelItem(
      m_dfgUICmdHandler,
      m_binding,
      m_execPath,
      m_exec,
      m_itemPath,
      portOrBlockName
      ) );
}

QVariant InstModelItem::getValue()
{
  return QVariant();
}

void InstModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  assert( false );
}

} // namespace ModelItems
} // namespace FabricUI
