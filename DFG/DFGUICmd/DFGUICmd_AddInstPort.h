// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGUICmd_AddInstPort__
#define __UI_DFG_DFGUICmd_AddInstPort__

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_Exec.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

class DFGUICmd_AddInstPort
  : public DFGUICmd_Exec
{
public:

  DFGUICmd_AddInstPort(
    FabricCore::DFGBinding const &binding,
    QString execPath,
    FabricCore::DFGExec const &exec,
    QString instName,
    QString desiredPortName,
    FabricCore::DFGPortType portType,
    QString typeSpec,
    QString pathToConnectWith,
    FabricCore::DFGPortType connectType,
    QString extDep,
    QString metaData
    )
    : DFGUICmd_Exec(
      binding,
      execPath,
      exec
      )
    , m_instName( instName.trimmed() )
    , m_desiredPortName( desiredPortName.trimmed() )
    , m_portType( portType )
    , m_typeSpec( typeSpec.trimmed() )
    , m_pathToConnectWith( pathToConnectWith.trimmed() )
    , m_connectType( connectType )
    , m_extDep( extDep.trimmed() )
    , m_metaData( metaData.trimmed() )
    {}

  static FTL::CStrRef CmdName()
    { return DFG_CMD_NAME("AddInstPort"); }

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
    FTL::CStrRef desiredPortName,
    FTL::CStrRef typeSpec,
    FTL::CStrRef pathToConnect,
    FTL::CStrRef extDep,
    FTL::CStrRef metaData,
    unsigned &coreUndoCount
    );

private:

  QString m_instName;
  QString m_desiredPortName;
  FabricCore::DFGPortType m_portType;
  QString m_typeSpec;
  QString m_pathToConnectWith;
  FabricCore::DFGPortType m_connectType;
  QString m_extDep;
  QString m_metaData;
  
  QString m_actualPortName;
};

FABRIC_UI_DFG_NAMESPACE_END

#endif // __UI_DFG_DFGUICmd_AddInstPort__
