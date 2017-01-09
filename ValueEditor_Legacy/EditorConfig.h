// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_EditorConfig__
#define __UI_ValueEditor_EditorConfig__

#include <QColor>
#include <QPen>
#include <QFont>

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {

    struct EditorConfig
    {
      QColor backgroundColor;
      int minLabelWidth;

      EditorConfig();
    };

  };

};

#endif // __UI_ValueEditor_EditorConfig__
