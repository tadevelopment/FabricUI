//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "RTValDictModelItem.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor; 
using namespace Application;
using namespace OptionsEditor;

RTValDictModelItem::RTValDictModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor *editor,
  RTVal options) 
  : BaseRTValModelItem(name, path)
{
  try
  {
    RTVal keys = options.getDictKeys();
    for(unsigned i = 0; i < keys.getArraySize(); i++) 
    {
      RTVal key = keys.getArrayElementRef(
        i); 

      RTVal childrenOptions = options.getDictElement(
        key); 

      std::string childName = key.getStringCString();

      BaseRTValModelItem* item = editor->constructModel(
        childName,
        m_path,
        editor,
        childrenOptions);

      m_children[childName] = item;
      m_keys.push_back(childName);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValDictModelItem::RTValDictModelItem",
      "",
      e.getDesc_cstr());
  }
}
 
RTValDictModelItem::~RTValDictModelItem() 
{
}

int RTValDictModelItem::getNumChildren() 
{ 
  return m_children.size(); 
}

BaseModelItem* RTValDictModelItem::getChild(
  FTL::StrRef childName, 
  bool doCreate) 
{ 
  return m_children[childName.data()]; 
}

BaseModelItem* RTValDictModelItem::getChild(
  int index, 
  bool doCreate) 
{ 
  return m_children[m_keys[index]]; 
}

void RTValDictModelItem::resetToDefault() 
{
  std::map<std::string, BaseRTValModelItem*>::iterator it;
  for (it = m_children.begin(); it != m_children.end(); it++) 
    it->second->resetToDefault();
}

RTVal RTValDictModelItem::getRTValOptions()
{
  RTVal options;

  try
  {
    options = RTVal::ConstructDict(
      FabricApplicationStates::GetAppStates()->getContext(),
      "String",
      "RTVal");

    std::map<std::string, BaseRTValModelItem*>::iterator it;
    for(it = m_children.begin(); it != m_children.end(); it++) 
    {
      RTVal key = RTVal::ConstructString(
        FabricApplicationStates::GetAppStates()->getContext(),
        it->first.data());

      BaseRTValModelItem *child = (BaseRTValModelItem *)it->second;
 
      options.setDictElement(
        key,
        RTValUtil::forceToKLRTVal(child->getRTValOptions())
        );
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValDictModelItem::getRTValOptions",
      "",
      e.getDesc_cstr());
  }
  
  return options;
}

void RTValDictModelItem::setRTValOptions(
  RTVal options) 
{
  try
  {
    options = RTValUtil::forceToRTVal(options);

    if(!options.isDict())
      throw("RTValDictModelItem::setRTValOptions, options is not a dictionay");

    std::map<std::string, BaseRTValModelItem*>::iterator it;
    for(it=m_children.begin(); it!=m_children.end(); it++) 
    {
      RTVal key = RTVal::ConstructString(
        options.getContext(),
        it->first.data());

      RTVal childrenOptions = options.getDictElement(
        key); 

      BaseRTValModelItem *child = (BaseRTValModelItem *)it->second;
       
      child->setRTValOptions(childrenOptions);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValDictModelItem::setRTValOptions",
      "",
      e.getDesc_cstr());
  }
}
