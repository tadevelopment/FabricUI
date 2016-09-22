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

QVariant InstBlockPortModelItem::getValue()
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

bool InstBlockPortModelItem::hasDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getPortResolvedType( m_portPath.c_str() );
  return (ctype != NULL);
}

void InstBlockPortModelItem::resetToDefault()
{
//#pragma message("Fix instance values for non-arg ports")
  //// If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getPortResolvedType( m_portPath.c_str() );
  if (ctype != NULL)
  {
    FabricCore::RTVal val =
      m_exec.getPortDefaultValue( m_portPath.c_str(), ctype );
    if ( val.isValid() )
      onViewValueChanged( QVariant::fromValue( val ) );
  }
}

} // namespace ModelItems
} // namespace FabricUI
