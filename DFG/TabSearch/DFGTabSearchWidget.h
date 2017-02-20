// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_DFGTabSearchWidget__
#define __UI_DFG_TabSearch_DFGTabSearchWidget__

#include <FabricUI/DFG/DFGTabSearchWidget.h>

namespace FabricUI
{
  namespace DFG
  {
    class DFGTabSearchWidget2 : public DFGAbstractTabSearchWidget
    {
    public:

      DFGTabSearchWidget2() {};

      void showForSearch( QPoint globalPos ) FTL_OVERRIDE { show(); };
    };
  };

};

#endif // __UI_DFG_TabSearch_DFGTabSearchWidget__
