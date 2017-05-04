//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "BaseRTValOptionsEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include "RTValDictModelItem.h"

using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Util;

RTValDictModelItem::RTValDictModelItem(
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
    RTVal keys = rtValOptions->getDictKeys();

    for (unsigned i = 0; i < keys.getArraySize(); i++) 
    {
      RTVal key = keys.getArrayElementRef(
        i); 

      RTVal chidOptions = rtValOptions->getDictElement(
        key); 

      constructModel(
        key.getStringCString(),
        editor,
        (void*)&chidOptions,
        settings);
    }
  }

  catch(Exception &e)
  {
    printf(
      "RTValDictModelItem::RTValDictModelItem: exception: %s\n", 
      e.getDesc_cstr());
  }
}
 
RTValDictModelItem::~RTValDictModelItem() 
{
}

RTVal RTValDictModelItem::getRTValOptions()
{
  RTVal options;

  try
  {
    options = RTVal::ConstructDict(
      m_client,
      "String",
      "RTVal");

    std::map<std::string, BaseModelItem*>::iterator it;
    for(it = m_children.begin(); it != m_children.end(); it++) 
    {
      RTVal keyVal = RTVal::ConstructString(
        m_client,
        it->first.data());

      BaseModelItem *child = (BaseModelItem *)it->second;
      RTValItem *rtValChild = dynamic_cast<RTValItem*>(
        child);

      options.setDictElement(
        keyVal,
        Util::RTValUtil::rtValToKLRTVal(
          rtValChild->getRTValOptions())
        );
    }
  }

  catch(Exception &e)
  {
    printf(
      "RTValDictModelItem::getRTValOptions: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return options;
}

void RTValDictModelItem::setRTValOptions(
  RTVal options) 
{
  try
  {
    if(options.isWrappedRTVal()) 
      options = options.getUnwrappedRTVal(); 

    if(!options.isDict())
      throw("RTValDictModelItem::setRTValOptions, options is not a dictionay");

    unsigned count = 0;
    RTVal keys = options.getDictKeys();

    std::map<std::string, BaseModelItem*>::iterator it;
    for(it=m_children.begin(); it!=m_children.end(); it++) 
    {
      BaseModelItem * child = (BaseModelItem *)it->second;
      
      RTValItem *rtValChild = 
        dynamic_cast<RTValItem*>(child);

      RTVal chidOptions = options.getDictElement(
        keys.getArrayElementRef(count)
        ); 

      rtValChild->setRTValOptions(
        chidOptions);

      count++;
    }
  }

  catch(Exception &e)
  {
    printf(
      "RTValDictModelItem::setRTValOptions: exception: %s\n", 
      e.getDesc_cstr());
  }
}
