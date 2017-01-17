// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "PresetTreeItem.h"
#include "PresetTreeWidget.h"
#include <FabricUI/DFG/DFGController.h>
#include <FabricCore.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeItem::PresetTreeItem(
  FTL::CStrRef name
  )
  : TreeView::TreeItem( name )
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

QVariant PresetTreeItem::data(int role)
{
  if(role == Qt::ToolTipRole)
  {
    QObject *treeModel = model();
    PresetTreeWidget *treeWidget = (PresetTreeWidget*)treeModel->parent();
    if(treeWidget)
    {
      FabricCore::DFGHost &host = treeWidget->getController()->getHost();
      return QString(host.getPresetMetadata(path().c_str(), "uiTooltip"));
    }
  }
  return TreeItem::data(role);
}
