//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/ExecBlockPortItemMetadata.h>
#include <FabricUI/ModelItems/ExecBlockPortModelItem.h>
#include <FabricUI/ModelItems/RootModelItem.h>
#include <QStringList>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////
ExecBlockPortModelItem::ExecBlockPortModelItem(
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

ExecBlockPortModelItem::~ExecBlockPortModelItem()
{
}

FabricUI::ValueEditor::ItemMetadata *ExecBlockPortModelItem::getMetadata()
{
  if ( !m_metadata )
    m_metadata = new ExecBlockPortItemMetadata( this );

  return m_metadata;
}

QVariant ExecBlockPortModelItem::getValue()
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
  catch ( FabricCore::Exception e )
  {
    reportFabricCoreException( e );
  }
  return QVariant();
}

FTL::CStrRef ExecBlockPortModelItem::getName()
{
  return m_portName;
}

bool ExecBlockPortModelItem::canRename()
{
  return false;
}

void ExecBlockPortModelItem::rename( FTL::CStrRef newName )
{
  assert( false );
}

void ExecBlockPortModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( m_portName == oldName );
  m_portName = newName;
  updatePortPath();
}

bool ExecBlockPortModelItem::hasDefault()
{
  // If we have a resolved type, allow getting the default val
  const char* ctype = m_exec.getPortResolvedType( m_portPath.c_str() );
  return (ctype != NULL);
}

void ExecBlockPortModelItem::resetToDefault()
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
