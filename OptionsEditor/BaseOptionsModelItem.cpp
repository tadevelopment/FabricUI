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
{
	// Don't add the separator at
	// the begining of the path.
 	m_path = (path.length() > 0)
		?	path + path_Separator + name
		: name;
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
