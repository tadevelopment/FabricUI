// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_Style_FabricStyle__
#define __UI_Style_FabricStyle__

#include <QProxyStyle>
#include <QStyleFactory>

namespace FabricUI
{

  namespace Style
  {

    class FabricStyle : public QProxyStyle
    {
        Q_OBJECT

    public:
        FabricStyle() : QProxyStyle(QStyleFactory::create("windows")) {}

        virtual void polish(QPalette &palette);
    };

  };

};

#endif // __UI_Style_FabricStyle__
