// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariableTreeItem__
#define __UI_DFG_VariableTreeItem__

#include <FabricUI/TreeView/TreeItem.h>

namespace FabricUI
{

  namespace DFG
  {

    class VariableTreeItem : public TreeView::TreeItem
    {

    public:

      VariableTreeItem(char const * name, char const * path);

      virtual FTL::CStrRef type() const { return FTL_STR("Variable"); }
      virtual std::string path() const { return m_path; }
      virtual Qt::ItemFlags flags();
      virtual QString mimeDataAsText();

    private:
        std::string m_path;
    };

  };

};

#endif // __UI_DFG_VariableTreeItem__
