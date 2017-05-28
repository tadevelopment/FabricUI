//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <QString>
#include "RTValArrayModelItem.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor; 
using namespace Application;
using namespace OptionsEditor;

RTValArrayModelItem::RTValArrayModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor *editor,
  RTVal options) 
  : BaseRTValModelItem(name, path)
{
  try
  {
    for(unsigned i=0; i<options.getArraySize(); i++) 
    {
      RTVal childrenOptions = options.getArrayElementRef(
        i); 

      std::string childName = name + "_" + std::string(QString::number(i).toUtf8().constData());
      
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
      "RTValArrayModelItem::RTValArrayModelItem",
      "",
      e.getDesc_cstr());
  }
}
 
RTValArrayModelItem::~RTValArrayModelItem() 
{
}

int RTValArrayModelItem::getNumChildren() 
{ 
  return m_children.size(); 
}

BaseModelItem* RTValArrayModelItem::getChild(
  FTL::StrRef childName, 
  bool doCreate) 
{ 
  return m_children[childName.data()]; 
}

BaseModelItem* RTValArrayModelItem::getChild(
  int index, 
  bool doCreate) 
{ 
  return m_children[m_keys[index]]; 
}

void RTValArrayModelItem::resetToDefault() 
{
  std::map<std::string, BaseRTValModelItem*>::iterator it;
  for (it = m_children.begin(); it != m_children.end(); it++) 
    it->second->resetToDefault();
}

RTVal RTValArrayModelItem::getRTValOptions()
{
  RTVal options;

  try
  {
    options = RTVal::ConstructVariableArray(
      FabricApplicationStates::GetAppStates()->getContext(),
      "RTVal");

    options.setArraySize(
      m_children.size());

    unsigned count = 0;
    
    std::map<std::string, BaseRTValModelItem*>::iterator it;
    for(it=m_children.begin(); it!=m_children.end(); it++) 
    {
      BaseRTValModelItem* child = (BaseRTValModelItem*)it->second;
     
      options.setArrayElement(
        count,
        RTValUtil::rtValToKLRTVal(child->getRTValOptions()) 
        );

      count++;
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValArrayModelItem::getRTValOptions",
      "",
      e.getDesc_cstr());
  }

  return options;
}

void RTValArrayModelItem::setRTValOptions(
  RTVal options) 
{  
  try
  { 
    options = RTValUtil::klRTValToRTVal(options);

    if(!options.isArray())
      FabricException::Throw(
        "RTValArrayModelItem::setRTValOptions",
        "options is not an array");

    unsigned count = 0;

    std::map<std::string, BaseRTValModelItem*>::iterator it;
    for(it=m_children.begin(); it!=m_children.end(); it++) 
    {
      BaseRTValModelItem* child = (BaseRTValModelItem*)it->second;
 
      RTVal childrenOptions = options.getArrayElementRef(count);
      
      child->setRTValOptions(childrenOptions);
      
      count++;
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValArrayModelItem::setRTValOptions",
      "",
      e.getDesc_cstr());
  }
}
