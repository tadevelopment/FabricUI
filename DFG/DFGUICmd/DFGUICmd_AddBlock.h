// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddBlock__
#define __UI_DFG_DFGUICmd_AddBlock__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddBlock
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddBlock(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString desiredName,
    QPointF pos
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_desiredName( desiredName.trimmed() )
    , m_pos( pos )
    {}

  QString getActualName() const
  {
    assert( wasInvoked() );
    return m_actualName;
  }

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddBlock"); }

protected:

  QString getDesiredName()
    { return m_desiredName; }
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

private:

  QString m_desiredName;
  QPointF m_pos;
  
  QString m_actualName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif //__UI_DFG_DFGUICmd_AddBlock__
