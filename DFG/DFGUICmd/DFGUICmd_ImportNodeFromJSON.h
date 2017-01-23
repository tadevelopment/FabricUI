// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_ImportNodeFromJSON__
#define __UI_DFG_DFGUICmd_ImportNodeFromJSON__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddNode.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_ImportNodeFromJSON
  : public DFGUICmd_AddNode
{
public:

  DFGUICmd_ImportNodeFromJSON(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString nodeName,
    QString filePath,
    QPointF pos
    )
    : DFGUICmd_AddNode(
      binding,
      execPath,
      exec,
      nodeName,
      pos
      )
    {
      m_filePath = filePath;
    }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("ImportNode"); }

protected:

  QString getDesiredNodeName()
   { return getPrimaryArg(); }
  
  virtual void appendDesc( QString &desc );

  virtual FTL::CStrRef invokeAdd( unsigned &coreUndoCount );

  QString m_filePath;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_ImportNodeFromJSON__
