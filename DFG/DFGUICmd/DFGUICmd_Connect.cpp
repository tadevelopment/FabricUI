//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Connect.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_Connect::appendDesc( QString &desc )
{
  desc += "Connect ";
  appendDesc_Path( m_srcPaths.join("|"), desc );
  desc += " to ";
  appendDesc_Path( m_dstPaths.join("|"), desc );
}

void DFGUICmd_Connect::invoke( unsigned &coreUndoCount )
{
  QList<QByteArray> srcPathBAs;
  srcPathBAs.reserve( m_srcPaths.size() );
  foreach ( QString nodeName, m_srcPaths )
    srcPathBAs.push_back( nodeName.toUtf8() );

  std::vector<FTL::CStrRef> srcPaths;
  srcPaths.reserve( m_srcPaths.size() );
  foreach ( QByteArray srcPathBA, srcPathBAs )
    srcPaths.push_back( srcPathBA.constData() );

  QList<QByteArray> dstPathBAs;
  dstPathBAs.reserve( m_dstPaths.size() );
  foreach ( QString nodeName, m_dstPaths )
    dstPathBAs.push_back( nodeName.toUtf8() );

  std::vector<FTL::CStrRef> dstPaths;
  dstPaths.reserve( m_dstPaths.size() );
  foreach ( QByteArray dstPathBA, dstPathBAs )
    dstPaths.push_back( dstPathBA.constData() );

  invokeAdd(
    srcPaths,
    dstPaths,
    coreUndoCount
    );
}

void DFGUICmd_Connect::invokeAdd(
  FTL::ArrayRef<FTL::CStrRef> srcPaths,
  FTL::ArrayRef<FTL::CStrRef> dstPaths,
  unsigned &coreUndoCount
  )
{
  size_t num = ( srcPaths.size() <= dstPaths.size() ? srcPaths.size() : dstPaths.size() );
  for ( size_t i = 0; i < num; ++i )
  {
    getExec().connectTo(
      srcPaths[i].c_str(),
      dstPaths[i].c_str()
      );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
