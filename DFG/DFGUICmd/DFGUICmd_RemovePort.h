// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_RemovePort__
#define __UI_DFG_DFGUICmd_RemovePort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>
#include <FTL/ArrayRef.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_RemovePort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_RemovePort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QStringList portNames
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_portNames( portNames )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("RemovePort"); }

protected:
  
  virtual void appendDesc( QString &desc );
  
  virtual void invoke( unsigned &coreUndoCount );

  void invoke(
    FTL::ArrayRef<FTL::CStrRef> portNames,
    unsigned &coreUndoCount
    );

private:

  QStringList m_portNames;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_RemovePort__
