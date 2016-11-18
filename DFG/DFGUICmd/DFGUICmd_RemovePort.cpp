//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_RemovePort.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_RemovePort::appendDesc( QString &desc )
{
  desc += "Remove ";
  appendDesc_Path( m_portNames.join("|"), desc );
}

void DFGUICmd_RemovePort::invoke( unsigned &coreUndoCount )
{
  QList<QByteArray> portNameBAs;
  portNameBAs.reserve( m_portNames.size() );
  foreach ( QString portName, m_portNames )
    portNameBAs.push_back( portName.toUtf8() );

  std::vector<FTL::CStrRef> portNames;
  portNames.reserve( m_portNames.size() );
  foreach ( QByteArray portNameBA, portNameBAs )
    portNames.push_back( portNameBA.constData() );

  invoke(
    portNames,
    coreUndoCount
    );
}

void DFGUICmd_RemovePort::invoke(
  FTL::ArrayRef<FTL::CStrRef> portPaths,
  unsigned &coreUndoCount
  )
{
  for ( size_t i = 0; i < portPaths.size(); ++i )
  {
    getExec().removePort( portPaths[i].c_str() );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
