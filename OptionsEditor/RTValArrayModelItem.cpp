//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <QString>
#include "RTValArrayModelItem.h"
#include "BaseRTValOptionsEditor.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Util;

RTValArrayModelItem::RTValArrayModelItem(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor *editor,
  void *options,
  QSettings *settings) 
  : BaseListModelItem(name, path)
{
  try
  {
    BaseRTValOptionsEditor* rtValEditor = dynamic_cast<BaseRTValOptionsEditor*>(
      editor);

    m_client = rtValEditor->getClient();
    
    RTVal *rtValOptions = (RTVal*)options;

    for(unsigned i=0; i<rtValOptions->getArraySize(); i++) 
    {
      RTVal chidOptions = rtValOptions->getArrayElementRef(
        i); 

      constructModel(
        name + "_" + std::string(QString::number(i).toUtf8().constData()),
        editor,
        (void*)&chidOptions,
        settings);
    }
  }
  
  catch(Exception &e)
  {
    printf(
      "RTValArrayModelItem::RTValArrayModelItem: exception: %s\n", 
      e.getDesc_cstr());
  }
}
 
RTValArrayModelItem::~RTValArrayModelItem() 
{
}

RTVal RTValArrayModelItem::getRTValOptions()
{
  RTVal options;

  try
  {
    options = RTVal::ConstructVariableArray(
      m_client,
      "RTVal");

    options.setArraySize(
      m_children.size());

    unsigned count = 0;
    
    std::map<std::string, BaseModelItem*>::iterator it;
    for(it=m_children.begin(); it!=m_children.end(); it++) 
    {
      BaseModelItem * child = (BaseModelItem *)it->second;
      RTValItem *rtValChild = dynamic_cast<RTValItem*>(
        child);

      options.setArrayElement(
        count,
        Util::RTValUtil::rtValToKLRTVal(
          rtValChild->getRTValOptions())
        );

      count++;
    }
  }

  catch(Exception &e)
  {
    printf(
      "RTValArrayModelItem::getRTValOptions: exception: %s\n", 
      e.getDesc_cstr());
  }

  return options;
}

void RTValArrayModelItem::setRTValOptions(
  RTVal options) 
{  
  try
  { 
    if(options.isWrappedRTVal()) 
      options = options.getUnwrappedRTVal(); 

    if(!options.isArray())
      throw("RTValArrayModelItem::setRTValOptions, options is not an array");

    unsigned count = 0;

    std::map<std::string, BaseModelItem*>::iterator it;
    for(it=m_children.begin(); it!=m_children.end(); it++) 
    {
      BaseModelItem * child = (BaseModelItem *)it->second;
      
      RTValItem *rtValChild = 
        dynamic_cast<RTValItem*>(child);

      RTVal chidOptions = options.getArrayElementRef(count);
      
      rtValChild->setRTValOptions(chidOptions);
      
      count++;
    }
  }

  catch(Exception &e)
  {
    printf(
      "RTValArrayModelItem::setRTValOptions: exception: %s\n", 
      e.getDesc_cstr());
  }
}
