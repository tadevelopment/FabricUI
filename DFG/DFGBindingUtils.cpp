//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGBindingUtils.h"
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGBindingUtils::DFGBindingUtils() {
}

QStringList DFGBindingUtils::getVariableWordsFromBinding(
  FabricCore::DFGBinding &binding, 
  FTL::CStrRef currentExecPath,
  QStringList varTypes)
{
  QStringList words;

  FabricCore::DFGStringResult json =  binding.getVars();
  FTL::JSONStrWithLoc jsonStrWithLoc( json.getCString() );
  FTL::OwnedPtr<FTL::JSONObject> jsonObject(
    FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
    );

  std::vector<FTL::CStrRef> execPaths;
  std::vector<std::string> prefixes;
  std::vector<FTL::JSONObject const *> objects;
  execPaths.push_back(currentExecPath);
  prefixes.push_back("");
  objects.push_back(jsonObject.get());

  for(size_t i=0;i<objects.size();i++)
  {
    FTL::JSONObject const * varsObject = objects[i]->maybeGetObject( FTL_STR("vars") );
    if(varsObject)
    {
      for(FTL::JSONObject::const_iterator it = varsObject->begin();
        it != varsObject->end(); it++
        )
      {
        FTL::CStrRef key = it->first;
        std::string path = prefixes[i];

        if(path.length() > 0)
          path += ".";

        path += key.c_str();
        if(words.contains(path.c_str()))
          continue;

        /// If varTypes list is empty, we don't check the variable type
        if(varTypes.size() == 0) 
          words.append(path.c_str());

        /// Otherwise, check if the type of the current variable is listed.
        else
        {
          FTL::JSONObject const *value = it->second->cast<FTL::JSONObject>();
          for(FTL::JSONObject::const_iterator jt = value->begin(); jt != value->end(); jt++) 
          {
            for(int j=0; j<varTypes.size(); ++j)
            {
              if(QString(jt->second->getStringValue().c_str()) == varTypes[j])
                words.append(path.c_str());
            }
          }
        }
      }
    }

    FTL::JSONObject const * subsObject = objects[i]->maybeGetObject( FTL_STR("subs") );
    if(subsObject)
    {
      for(FTL::JSONObject::const_iterator it = subsObject->begin();
        it != subsObject->end(); it++
        )
      {
        FTL::JSONObject const * subGraph = it->second->maybeCast<FTL::JSONObject>();
        if(subGraph)
        {
          std::string path;
          FTL::CStrRef execPath = execPaths[i];
          FTL::CStrRef graphName = it->first;
          if(execPath.size() > 0)
          {
            std::string graphNameStr(graphName);
            std::string execPathStr(execPath);
            if(graphNameStr == execPathStr || 
              graphNameStr + "." == execPathStr.substr(0, graphNameStr.length() + 1))
            {
              execPath = execPath.substr(graphNameStr.length() + 1).data();
            }
            else
            {
              path = graphName;
              execPath = "";
            }
          }
          else
          {
            path = prefixes[i];
            if(path.length() > 0)
              path += ".";
            path += std::string(it->first);
          }

          execPaths.push_back(execPath);
          prefixes.push_back(path);
          objects.push_back(subGraph);
        }
      }      
    }
  }

  return words;
}

QList<FabricCore::RTVal> DFGBindingUtils::getVariableValuesFromBinding(
  FabricCore::DFGBinding &binding,
  FTL::CStrRef currentExecPath,
  QStringList varTypes)
{
  QList<FabricCore::RTVal> varValueList;
  foreach(QString valName, getVariableWordsFromBinding(binding, currentExecPath, varTypes))
  {
    if(binding.getExec().hasVar(valName.toUtf8().constData()))
      varValueList.append(binding.getExec().getVarValue(valName.toUtf8().constData()));
  }
  return varValueList;
}

QStringList DFGBindingUtils::getVariableWordsFromBinding(
  FabricCore::DFGBinding &binding, 
  FTL::CStrRef currentExecPath)
{
  QStringList varTypes;
  return getVariableWordsFromBinding(binding, currentExecPath, varTypes);
}

QString DFGBindingUtils::getPresetPathFromNode(
  FabricCore::DFGExec &exec, 
  QString nodeName) 
{
  QString presetPath;

  try
  {
    FabricCore::DFGStringResult desc = exec.getDesc();
    char const *descData; uint32_t descSize;
    desc.getStringDataAndLength( descData, descSize );

    FTL::JSONStrWithLoc jsonSrcWithLoc( FTL::StrRef( descData, descSize ) );

    FTL::OwnedPtr<FTL::JSONValue const> execValue( FTL::JSONValue::Decode( jsonSrcWithLoc ) );
    FTL::JSONObject const *execObject = execValue->cast<FTL::JSONObject>();

    if ( execObject->getString( FTL_STR("objectType") ) == FTL_STR("Graph") )
    {
      FTL::JSONArray const *nodesArray = execObject->get( FTL_STR("nodes") )->cast<FTL::JSONArray>();
      for ( size_t i = 0; i < nodesArray->size(); ++i )
      {
        FTL::JSONObject const *nodeObject = nodesArray->get( i )->cast<FTL::JSONObject>();
        if(nodeObject->getString( FTL_STR("name") ) == nodeName.toUtf8().data())
        {
          if(nodeObject->has(FTL_STR("presetPath")))
            presetPath = QString(nodeObject->getString( FTL_STR("presetPath") ).c_str());
        }
      }
    }
  }
  catch ( FTL::JSONException je )
  {
    printf(" DFGBindingUtils::getPresetPathFromNode : Caught JSONException: %s\n", je.getDescCStr() );
  } 

  return presetPath;
}
