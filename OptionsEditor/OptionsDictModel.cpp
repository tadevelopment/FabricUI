//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "OptionsModel.h"
#include "OptionsDictModel.h"

using namespace FabricUI;
using namespace OptionsEditor ;
using namespace ValueEditor;

const char namePath_Separator = '/';

OptionsDictModel::OptionsDictModel(
  const QString &name,
  FabricCore::RTVal dict,
  QSettings* settings,
  const QString &namePath,
  BaseOptionsEditor* editor) 
  : m_name(name)
  , m_namePath(namePath + namePath_Separator + name)
{
  FabricCore::RTVal keys = dict.getDictKeys();

  for (unsigned i = 0; i < keys.getArraySize(); i++) 
  {
    FabricCore::RTVal key = keys.getArrayElementRef(i);
    FabricCore::RTVal value = dict.getDictElement(key);
    if (value.isWrappedRTVal()) 
      value = value.getUnwrappedRTVal(); 

    BaseModelItem* item = NULL;
    if (value.isDict() && value.dictKeyHasType("String") && value.dictValueHasType("RTVal")) 
    {
      item = new OptionsDictModel(
        key.getStringCString(),
        value,
        settings,
        m_namePath,
        editor
      );
    }

    else 
    {
      item = new OptionsModel(
        key.getStringCString(),
        value,
        settings,
        m_namePath,
        editor
      );
    }

    m_children[key.getStringCString()] = item;

    m_keys.push_back(key.getStringCString());
  }
}

OptionsDictModel::~OptionsDictModel() 
{
  for (std::map<QString, BaseModelItem*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
    delete it->second;
  }
}

FTL::CStrRef OptionsDictModel::getName() { 
  return m_name.toUtf8().constData(); 
}

int OptionsDictModel::getNumChildren() 
{ 
  return m_children.size(); 
}

BaseModelItem* OptionsDictModel::getChild(
  FTL::StrRef childName, 
  bool doCreate) 
{ 
  return m_children[childName.data()]; 
}

BaseModelItem* OptionsDictModel::getChild(
  int index, 
  bool doCreate) 
{ 
  return m_children[m_keys[index]]; 
}

bool OptionsDictModel::hasDefault() 
{
  return true; 
}

void OptionsDictModel::resetToDefault() 
{
  for (std::map<QString, BaseModelItem*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
    it->second->resetToDefault();
  }
}
