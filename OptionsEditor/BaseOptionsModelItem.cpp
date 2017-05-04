//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseOptionsModelItem.h"

using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;
 
const char path_Separator = '/';

BaseOptionsModelItem::BaseOptionsModelItem(
  const std::string &name,
  const std::string &path) 
  : BaseModelItem()
  , m_name(name)
  , m_path(path + path_Separator + name)
{
}

BaseOptionsModelItem::~BaseOptionsModelItem()
{
}

bool BaseOptionsModelItem::hasDefault() 
{ 
  return true; 
}

FTL::CStrRef BaseOptionsModelItem::getName() 
{ 
  return m_name; 
}

FTL::CStrRef BaseOptionsModelItem::getPath() 
{ 
  return m_path; 
}
