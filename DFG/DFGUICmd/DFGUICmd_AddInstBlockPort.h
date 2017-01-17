// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddInstBlockPort__
#define __UI_DFG_DFGUICmd_AddInstBlockPort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddInstBlockPort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddInstBlockPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString instName,
    QString blockName,
    QString desiredPortName,
    QString typeSpec,
    QString pathToConnectWith,
    QString extDep,
    QString metaData
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_instName( instName.trimmed() )
    , m_blockName( blockName.trimmed() )
    , m_desiredPortName( desiredPortName.trimmed() )
    , m_typeSpec( typeSpec.trimmed() )
    , m_pathToConnectWith( pathToConnectWith.trimmed() )
    , m_extDep( extDep.trimmed() )
    , m_metaData( metaData.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddInstBlockPort"); }

  QString getActualPortName()
  {
    assert( wasInvoked() );
    return m_actualPortName;
  }

protected:

  virtual void appendDesc( QString &desc );

  virtual void invoke( unsigned &coreUndoCount );

  FTL::CStrRef invoke(
    FTL::CStrRef execPath,
    FTL::CStrRef instName,
    FTL::CStrRef blockName,
    FTL::CStrRef desiredPortName,
    FTL::CStrRef typeSpec,
    FTL::CStrRef pathToConnect,
    FTL::CStrRef extDep,
    FTL::CStrRef metaData,
    unsigned &coreUndoCount
    );

private:

  QString m_instName;
  QString m_blockName;
  QString m_desiredPortName;
  QString m_typeSpec;
  QString m_pathToConnectWith;
  QString m_extDep;
  QString m_metaData;
  
  QString m_actualPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddInstBlockPort__
