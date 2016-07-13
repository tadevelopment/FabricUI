//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/ModelItems/ExecBlockModelItem.h>
#include <FabricUI/ModelItems/ExecBlockPortModelItem.h>

namespace FabricUI {
namespace ModelItems {

//////////////////////////////////////////////////////////////////////////

ExecBlockModelItem::ExecBlockModelItem(
  DFG::DFGUICmdHandler *dfgUICmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  FTL::StrRef execBlockName
  )
 : ItemModelItem(
    dfgUICmdHandler,
    binding,
    execPath,
    exec,
    execBlockName
    )
{
}

ExecBlockModelItem::~ExecBlockModelItem()
{
}

FabricUI::ValueEditor::BaseModelItem *
ExecBlockModelItem::createChild( FTL::StrRef portName )
{
  return pushChild( new ExecBlockPortModelItem(
    m_dfgUICmdHandler,
    m_binding,
    m_execPath,
    m_exec,
    m_itemPath,
    portName
    ) );
}

QVariant ExecBlockModelItem::getValue()
{
  return QVariant();
}

void ExecBlockModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  assert( false );
}

} // namespace ModelItems
} // namespace FabricUI
