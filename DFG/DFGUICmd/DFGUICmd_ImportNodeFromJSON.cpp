// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ImportNodeFromJSON.h>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ImportNodeFromJSON::appendDesc( QString &desc )
{
  desc += "Import node ";
  appendDesc_NodeName( getActualNodeName(), desc );
}

FTL::CStrRef DFGUICmd_ImportNodeFromJSON::invokeAdd( unsigned &coreUndoCount )
{
  FTL::CStrRef actualNodeName;

  QFile file( m_filePath );
  if( file.open( QFile::ReadOnly ) )
  {
    QTextStream stream( &file );
    QString jsonVar = stream.readAll();

    try
    {
      actualNodeName = getExec().addInstFromJSON( jsonVar.toUtf8().constData() );
      ++coreUndoCount;
  
      actualNodeName = getExec().renameNode(actualNodeName.data(), getDesiredNodeName().toUtf8().constData());
      ++coreUndoCount;
    }
    catch(FabricCore::Exception e)
    {
      printf( "Caught FabricCore: %s\n", e.getDesc_cstr() );
    }
  }
  return actualNodeName;
}

FABRIC_UI_DFG_NAMESPACE_END
