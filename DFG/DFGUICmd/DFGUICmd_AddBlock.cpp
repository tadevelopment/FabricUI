// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddBlock.h>

#include <FTL/JSONEnc.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddBlock::appendDesc( QString &desc )
{
  desc += "Add block ";
  appendDesc_NodeName( getActualName(), desc );
}

void DFGUICmd_AddBlock::invoke(
  unsigned &coreUndoCount
  )
{
  std::string posJSONString;
  {
    FTL::JSONEnc<std::string> je( posJSONString, FTL::JSONFormat::Packed() );
    FTL::JSONObjectEnc<std::string> joe( je );
    {
      FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
      FTL::JSONFloat64Enc<std::string> xJFE( xJE, m_pos.x() );
    }
    {
      FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
      FTL::JSONFloat64Enc<std::string> yJFE( yJE, m_pos.y() );
    }
  }

  char const *metadataKeys[] = { "uiGraphPos" };
  char const *metadataValues[] = { posJSONString.c_str() };

  FTL::CStrRef actualNameCStr =
    getExec().addBlock(
      m_desiredName.toUtf8().constData(),
      1, // metadataCount
      &metadataKeys[0],
      &metadataValues[0]
      );
  ++coreUndoCount;
  m_actualName =
    QString::fromUtf8( actualNameCStr.data(), actualNameCStr.size() );
}

FABRIC_UI_DFG_NAMESPACE_END
