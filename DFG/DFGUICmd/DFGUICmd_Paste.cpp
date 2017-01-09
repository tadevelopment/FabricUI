//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Paste.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Paste::appendDesc( QString &desc )
{
  desc += "Paste JSON as ";
  appendDesc_NodeNames( m_pastedItemNames, desc );
}

void DFGUICmd_Paste::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_json.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_Paste::invoke(
  FTL::CStrRef json,
  unsigned &coreUndoCount
  )
{
  FabricCore::String newItemNamesJSON =
    getExec().importItemsJSON( json.c_str() );
  ++coreUndoCount;

  m_pastedItemNames =
    adjustNewItems(
      newItemNamesJSON,
      m_cursorPos,
      coreUndoCount
      );
}

FABRIC_UI_DFG_NAMESPACE_END
