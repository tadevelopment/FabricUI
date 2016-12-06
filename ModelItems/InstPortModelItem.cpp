//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/InstPortItemMetadata.h>
#include <FabricUI/ModelItems/InstPortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
InstPortModelItem::InstPortModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef nodeName,
  FTL::StrRef portName
  )
  : ItemPortModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    nodeName,
    portName
    )
{
}

InstPortModelItem::~InstPortModelItem()
{
}

FabricUI::ValueEditor::ItemMetadata *InstPortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new InstPortItemMetadata( this );

  return m_metadata;
}

QVariant InstPortModelItem::getValue()
{
  try
  {
    // TODO: Find a way to show values of connected ports
    if (m_exec.hasSrcPorts( m_portPath.c_str() ))
      return QVariant();

    // If we have a resolved type, allow getting the default val
    const char* ctype = m_exec.getPortResolvedType( m_portPath.c_str() );
    if (ctype != NULL)
    {
      FabricCore::RTVal rtVal = 
        m_exec.getPortResolvedDefaultValue( m_portPath.c_str(), ctype );
      assert( rtVal.isValid() );
      return QVariant::fromValue<FabricCore::RTVal>( rtVal.copy() );
    }
  }
  catch (FabricCore::Exception* e)
  {
    printf( "[ERROR] %s", e->getDesc_cstr() );
  }
  return QVariant();
}

FTL::CStrRef InstPortModelItem::getName()
{
  return m_portName;
}

bool InstPortModelItem::canRename()
{
  return !m_exec.isExecBlock( m_itemPath.c_str() )
    && !m_exec.isDepsPort( m_portPath.c_str() )
    && !m_exec.getSubExec( m_itemPath.c_str() ).editWouldSplitFromPreset();
}

void InstPortModelItem::rename( FTL::CStrRef newName )
{
  assert( canRename() );
  
  m_dfgUICmdHandler->dfgDoRenamePort(
    m_binding,
    QString::fromUtf8( m_execPath.data(), m_execPath.size() ),
    m_exec,
    QString::fromUtf8( m_portPath.data(), m_portPath.size() ),
    QString::fromUtf8( newName.data(), newName.size() )
    );
}

void InstPortModelItem::onRenamed(
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
