// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddInstPort.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>
#include <Persistence/RTValToJSONEncoder.hpp>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddInstPort::appendDesc( QString &desc )
{
  desc += "Add ";
  appendDesc_PortPath( getActualPortName(), desc );
  desc += " to ";
  appendDesc_NodeName( m_instName, desc );
}

void DFGUICmd_AddInstPort::invoke( unsigned &coreUndoCount )
{
  m_actualPortName = QString::fromUtf8(
    invoke(
      getExecPath().toUtf8().constData(),
      m_instName.toUtf8().constData(),
      m_desiredPortName.toUtf8().constData(),
      m_typeSpec.toUtf8().constData(),
      m_pathToConnectWith.toUtf8().constData(),
      m_extDep.toUtf8().constData(),
      m_metaData.toUtf8().constData(),
      coreUndoCount
      ).c_str()
    );
}

FTL::CStrRef DFGUICmd_AddInstPort::invoke(
  FTL::CStrRef execPath,
  FTL::CStrRef instName,
  FTL::CStrRef desiredPortName,
  FTL::CStrRef typeSpec,
  FTL::CStrRef pathToConnect,
  FTL::CStrRef extDep,
  FTL::CStrRef metaData,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGExec &exec = getExec();
  FabricCore::DFGExec instExec = exec.getSubExec( instName.c_str() );

  if ( !extDep.empty() )
  {
    instExec.addExtDep( extDep.c_str() );
    ++coreUndoCount;
  }

  // [pzion 20160226] This needs to live at least as long as metadataKeys
  // and metadataValues, since this owns the storage they refer to
  FTL::OwnedPtr<FTL::JSONObject> jo;

  unsigned metadataCount = 0;
  char const **metadataKeys = NULL;
  char const **metadataValues = NULL;

  if ( !metaData.empty() )
  {
    try
    {
      FTL::JSONStrWithLoc swl( metaData );
      jo = FTL::JSONValue::Decode( swl )->cast<FTL::JSONObject>();

      metadataCount = jo->size();
      metadataKeys = (char const **)alloca( metadataCount * sizeof( char const *) );
      metadataValues = (char const **)alloca( metadataCount * sizeof( char const *) );

      FTL::JSONObject::const_iterator it = jo->begin();
      unsigned index = 0;
      for(;it!=jo->end();it++)
      {
        metadataKeys[index] = it->first.c_str();
        metadataValues[index] = it->second->getStringValue().c_str();
        ++index;
      }
    }
    catch(FTL::JSONException e)
    {
      printf("DFGUICmd_AddInstPort: Json exception: '%s'\n", e.getDescCStr());
    }
  }

  FTL::CStrRef portName =
    instExec.addExecPortWithMetadata(
      desiredPortName.c_str(),
      m_portType,
      metadataCount,
      metadataKeys,
      metadataValues,
      typeSpec.c_str()
      );
  ++coreUndoCount;

  if ( !pathToConnect.empty() )
  {
    std::string instPortPath = instName;
    instPortPath += '.';
    instPortPath += portName;

    FabricCore::DFGPortType pathToConnectNodePortType =
      exec.getNodePortType( pathToConnect.c_str() );

    if ( pathToConnectNodePortType == FabricCore::DFGPortType_In )
    {
      std::pair<FTL::StrRef, FTL::CStrRef> split = pathToConnect.rsplit('.');
      std::string nodeToConnect = split.first;

      if ( !nodeToConnect.empty()
        && exec.getNodeType( nodeToConnect.c_str() )
          == FabricCore::DFGNodeType_Inst )
      {
        FTL::CStrRef nodePortToConnect = split.second;

        FTL::CStrRef resolvedType =
          exec.getNodePortResolvedType( pathToConnect.c_str() );
        if ( !resolvedType.empty() )
        {
          FabricCore::RTVal defaultValue =
            exec.getInstPortResolvedDefaultValue(
              pathToConnect.c_str(),
              resolvedType.c_str()
              );
          if ( defaultValue.isValid() )
          {
            exec.setPortDefaultValue( instPortPath.c_str(), defaultValue, true );
            ++coreUndoCount;
          }
        }

        FabricCore::DFGExec nodeToConnectExec =
          exec.getSubExec( nodeToConnect.c_str() );

        char const *metadatasToCopy[5] =
        {
          "uiRange",
          "uiCombo",
          DFG_METADATA_UIPERSISTVALUE
        };

        for ( unsigned i = 0; i < 5; ++i )
        {
          instExec.setExecPortMetadata(
            portName.c_str(),
            metadatasToCopy[i],
            nodeToConnectExec.getExecPortMetadata(
              nodePortToConnect.c_str(),
              metadatasToCopy[i]
              ),
            true
            );
          ++coreUndoCount;
        }
      }
    }

    if ( m_connectType == FabricCore::DFGPortType_Out )
    {
      exec.connectTo( instPortPath.c_str(), pathToConnect.c_str() );
      ++coreUndoCount;
    }
    if ( m_connectType == FabricCore::DFGPortType_In )
    {
      exec.connectTo( pathToConnect.c_str(), instPortPath.c_str() );
      ++coreUndoCount;
    }
  }

  return portName;
}

FABRIC_UI_DFG_NAMESPACE_END
