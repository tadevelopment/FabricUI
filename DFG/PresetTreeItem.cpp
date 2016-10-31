// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "PresetTreeItem.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeItem::PresetTreeItem(
  //FTL::CStrRef path,
  FTL::CStrRef name,
  FTL::CStrRef tooltip
  )
  : TreeView::TreeItem( name, "", tooltip)
  //, m_path( path )
{
}

Qt::ItemFlags PresetTreeItem::flags()
{
  Qt::ItemFlags flags = TreeView::TreeItem::flags();
  flags = flags | Qt::ItemIsDragEnabled;
  return flags;
}

QString PresetTreeItem::mimeDataAsText()
{
  QString res = "{\"type\": \"DFGPreset\", \"path\": \"";
	res+= QString(path().c_str());
	res +="\"}";
	return res;
}
