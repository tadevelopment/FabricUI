// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"
#include <map>
#include <FTL/JSONValue.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

NameSpaceTreeItem::NameSpaceTreeItem(FabricCore::DFGHost const &coreDFGHost, char const * name, char const * ns, QStringList filters)
: TreeView::TreeItem(name)
, m_coreDFGHost(coreDFGHost)
, m_nameSpace(ns)
, m_filters(filters)
{
  m_validated = false;
}

unsigned int NameSpaceTreeItem::numChildren()
{
  if(!m_validated)
  {
    std::string prefix = m_nameSpace + ".";
    std::map<std::string, std::string> nameSpaceLookup;
    std::map<std::string, std::string> presetLookup;

    FabricCore::DFGStringResult jsonStringResult = m_coreDFGHost.getPresetDesc(m_nameSpace.c_str());
 

    char const *jsonCStr;
    uint32_t jsonSize;
    jsonStringResult.getStringDataAndLength( jsonCStr, jsonSize );

    //std::cout << "jsonCStr " << jsonCStr << std::endl;
    FTL::CStrRef jsonStr( jsonCStr, jsonSize );
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );

    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );
    FTL::JSONObject const *membersObject =
      jsonObject->get( FTL_STR("members") )->cast<FTL::JSONObject>();

    for ( FTL::JSONObject::const_iterator it = membersObject->begin();
      it != membersObject->end(); ++it )
    {
      FTL::CStrRef name = it->first;

      FTL::JSONObject const *memberObject =
        it->second->cast<FTL::JSONObject>();
      std::cout << "name " << name.c_str() << " has metadata " << memberObject->has(FTL_STR("metadata")) << " " << memberObject->size() << std::endl;

      FTL::CStrRef objectType =
        memberObject->getString( FTL_STR("objectType") );

  /*   FTL::JSONArray const *array =  memberObject->getArray(  FTL_STR("metadata") );
     if(array)
      std::cout << "Hourra I have an array" << std::endl;
      
      if ( FTL::JSONObject const *metadata =
        memberObject->maybeGetObject( FTL_STR("metadata") ) )
      {
        for ( FTL::JSONObject::const_iterator it = metadata->begin();
          it != metadata->end(); ++it )
        {
          FTL::CStrRef key = it->first;
          std::cout << "key " << key.c_str() << std::endl;
        
        }
      }
*/

      FTL::CStrRef uiTooltip =name;


      if(objectType == "Preset" && m_showsPresets)
      {
        presetLookup.insert(std::pair<std::string, std::string>(name, uiTooltip));
      }
      else if(objectType == FTL_STR("NameSpace"))
      {
        nameSpaceLookup.insert(std::pair<std::string, std::string>(name, prefix + name.c_str()));
      }
    }

    for(std::map<std::string, std::string>::iterator it=nameSpaceLookup.begin();it!=nameSpaceLookup.end();it++)
    {
      QStringList filters;
      QString search = QString(it->first.c_str()) + ".";
      for(int i=0;i<m_filters.length();i++)
      {
        if(!m_filters[i].startsWith(search))
          continue;
        filters.append(m_filters[i].right(m_filters[i].length() - search.length()));
      }
      if(filters.length() == 0 && m_filters.length() > 0)
        continue;
      NameSpaceTreeItem * item = new NameSpaceTreeItem(m_coreDFGHost, it->first.c_str(), it->second.c_str(), filters);
      item->setShowsPresets(m_showsPresets);
      addChild(item);
    }

    if(m_showsPresets)
    {
      for(std::map<std::string, std::string>::iterator it=presetLookup.begin();it!=presetLookup.end();it++)
      {
        if(!includeChildName(it->first.c_str()))
          continue;
        addChild(new PresetTreeItem(it->first.c_str(), it->second.c_str()));
      }
    }

    m_validated = true;
  }
  return TreeView::TreeItem::numChildren();
}

QStringList NameSpaceTreeItem::filters() const
{
  return m_filters;
}

void NameSpaceTreeItem::setFilters(QStringList f)
{
  m_filters = f;
}

bool NameSpaceTreeItem::includeChildName(QString name)
{
  QString start = name + ".";
  QString end = "." + name;

  for(int i=0;i<m_filters.length();i++)
  {
    if(m_filters[i] == name)
      return true;
    if(m_filters[i].startsWith(start))
      return true;
    if(m_filters[i].endsWith(end))
      return true;
  }
  return m_filters.length() == 0;
}
