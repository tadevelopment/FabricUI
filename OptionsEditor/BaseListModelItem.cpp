//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "BaseListModelItem.h"

using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;
 
BaseListModelItem::BaseListModelItem(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor* editor,
  void *options,
  QSettings *settings) 
  : BaseOptionsModelItem(name, path)
{
}
 
BaseListModelItem::~BaseListModelItem() 
{
  std::map<std::string, BaseModelItem*>::iterator it;
  for (it = m_children.begin(); it != m_children.end(); it++) 
    delete it->second;
}

int BaseListModelItem::getNumChildren() 
{ 
  return m_children.size(); 
}

BaseModelItem* BaseListModelItem::getChild(
  FTL::StrRef childName, 
  bool doCreate) 
{ 
  return m_children[childName.data()]; 
}

BaseModelItem* BaseListModelItem::getChild(
  int index, 
  bool doCreate) 
{ 
  return m_children[m_keys[index]]; 
}

void BaseListModelItem::resetToDefault() 
{
  std::map<std::string, BaseModelItem*>::iterator it;
  for (it = m_children.begin(); it != m_children.end(); it++) 
    it->second->resetToDefault();
}

void BaseListModelItem::constructModel(
  const std::string &name,
  BaseOptionsEditor *editor,
  void *options,
  QSettings *settings) 
{
  BaseModelItem* item = editor->constructModel(
    name,
    m_path,
    editor,
    options,
    settings);

  m_children[name] = item;
  m_keys.push_back(name); 
}
