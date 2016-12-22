// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_AddPort.h>

#include <FTL/JSONValue.h>
#include <FTL/JSONException.h>
#include <Persistence/RTValToJSONEncoder.hpp>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_AddPort::appendDesc( QString &desc )
{
  desc += "Add ";
  appendDesc_PortPath( getActualPortName(), desc );
}

void DFGUICmd_AddPort::invoke( unsigned &coreUndoCount )
{
  m_actualPortName = QString::fromUtf8(
    invoke(
      getExecPath().toUtf8().constData(),
      m_desiredPortName.toUtf8().constData(),
      m_typeSpec.toUtf8().constData(),
      m_portToConnectWith.toUtf8().constData(),
      m_extDep.toUtf8().constData(),
      m_metaData.toUtf8().constData(),
      coreUndoCount
      ).c_str()
    );
}

FTL::CStrRef DFGUICmd_AddPort::invoke(
  FTL::CStrRef execPath,
  FTL::CStrRef desiredPortName,
  FTL::CStrRef typeSpec,
  FTL::CStrRef portToConnectPath,
  FTL::CStrRef extDep,
  FTL::CStrRef metaData,
  unsigned &coreUndoCount
  )
{
  FabricCore::DFGNotifBracket notifBracket( getHost() );
  FabricCore::DFGBinding &binding = getBinding();
  FabricCore::DFGExec &exec = getExec();

  if ( !extDep.empty() )
  {
    exec.addExtDep( extDep.c_str() );
    ++coreUndoCount;
  }

  // [pzion 20160226] This needs to live at least as long as metadataKeys
  // and metadataValues, since this owns the storage they refer to
  FTL::OwnedPtr<FTL::JSONObject> jo;

  unsigned metadataCount = 0;
  char const **metadataKeys = NULL;
  char const **metadataValues = NULL;
  bool setAsPersistable = false;

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
        if ( !strcmp(metadataKeys[index], DFG_METADATA_UIPERSISTVALUE)
          && !strcmp(metadataValues[index], "true") )
          setAsPersistable = true;
        ++index;
      }
    }
    catch(FTL::JSONException e)
    {
      printf("DFGUICmd_AddPort: Json exception: '%s'\n", e.getDescCStr());
    }
  }

  FTL::CStrRef portName =
    exec.addExecPortWithMetadata(
      desiredPortName.c_str(),
      m_portType,
      metadataCount,
      metadataKeys,
      metadataValues,
      typeSpec.c_str()
      );
  ++coreUndoCount;

  // we should always bind an rt val, even if we are
  // going to create a connection lateron.
  // the dccs require this given they listen to the 
  // argTypeChanged notification.
  if ( execPath.empty()
    && !typeSpec.empty()
    && typeSpec.find('$') == typeSpec.end() )
  {
    FabricCore::DFGHost host = getHost();
    FabricCore::Context context = host.getContext();
    FabricCore::RTVal argValue =
      FabricCore::RTVal::Construct(
        context,
        typeSpec.c_str(),
        0,
        0
        );
    binding.setArgValue(
      portName.c_str(),
      argValue,
      true
      );
    ++coreUndoCount;
  }

  if ( !portToConnectPath.empty() )
  {
    FabricCore::DFGPortType portToConnectNodePortType =
      exec.getPortType( portToConnectPath.c_str() );
    if ( portToConnectNodePortType == FabricCore::DFGPortType_In )
    {
      FTL::CStrRef::Split split = portToConnectPath.rsplit('.');
      std::string portToConnectNodeName = split.first;
      FTL::CStrRef portToConnectName = split.second;
      if ( !portToConnectNodeName.empty() )
      {
        FTL::CStrRef resolvedType =
          exec.getPortResolvedType( portToConnectPath.c_str() );
        if ( !resolvedType.empty() )
        {
          FabricCore::RTVal defaultValue =
            exec.getPortResolvedDefaultValue(
              portToConnectPath.c_str(),
              resolvedType.c_str()
              ).clone();
          if ( defaultValue.isValid() )
          {
            if ( execPath.empty() )
              binding.setArgValue( portName.c_str(), defaultValue, true );
            else
              exec.setPortDefaultValue( portName.c_str(), defaultValue, true );
            ++coreUndoCount;
          }
        }


        static unsigned const metadatasToCopyCount = 5;
        char const *metadatasToCopy[metadatasToCopyCount] =
        {
          "uiRange",
          "uiCombo",
          "uiHidden",
          "uiOpaque",
          DFG_METADATA_UIPERSISTVALUE
        };

        if ( !exec.isExecBlock( portToConnectNodeName.c_str() )
          && ( exec.isInstBlock( portToConnectNodeName.c_str() )
            || exec.getNodeType( portToConnectNodeName.c_str() ) == FabricCore::DFGNodeType_Inst ) )
        {
          // In the specific case of instances, copy metadata from subexec
          
          FabricCore::DFGExec portToConnectSubExec =
            exec.getSubExec( portToConnectNodeName.c_str() );
          for ( unsigned i = 0; i < metadatasToCopyCount; ++i )
          {
            exec.setExecPortMetadata(
              portName.c_str(),
              metadatasToCopy[i],
              portToConnectSubExec.getPortMetadata(
                portToConnectName.c_str(),
                metadatasToCopy[i]
                ),
              true
              );
            ++coreUndoCount;
          }
        }
        else
        {
          for ( unsigned i = 0; i < metadatasToCopyCount; ++i )
          {
            exec.setExecPortMetadata(
              portName.c_str(),
              metadatasToCopy[i],
              exec.getPortMetadata(
                portToConnectPath.c_str(),
                metadatasToCopy[i]
                ),
              true
              );
            ++coreUndoCount;
          }
        }
      }
      if (setAsPersistable)
      {
        exec.setExecPortMetadata(
          portName.c_str(),
          DFG_METADATA_UIPERSISTVALUE,
          "true",
          true
          );
         ++coreUndoCount;
      }
    }

    if ( m_portType != FabricCore::DFGPortType_Out
      && portToConnectNodePortType != FabricCore::DFGPortType_Out )
    {
      exec.connectTo( portName.c_str(), portToConnectPath.c_str() );
      ++coreUndoCount;
    }
    if ( m_portType != FabricCore::DFGPortType_In )
    {
      exec.connectTo( portToConnectPath.c_str(), portName.c_str() );
      ++coreUndoCount;
    }
  }

  return portName;
}

FABRIC_UI_DFG_NAMESPACE_END
