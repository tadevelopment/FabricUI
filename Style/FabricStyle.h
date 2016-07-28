// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_Style_FabricStyle__
#define __UI_Style_FabricStyle__

#include <QStyle>

namespace FabricUI
{

  namespace Style
  {

    class FabricStyle : public QStyle
    {
        Q_OBJECT

    public:
        FabricStyle() {}

        virtual void polish(QPalette &palette);
        virtual QIcon standardIcon(QStyle::StandardPixmap, const QStyleOption*, const QWidget*) const;
        virtual int layoutSpacing(QSizePolicy::ControlType, QSizePolicy::ControlType, Qt::Orientation, const QStyleOption*, const QWidget*) const;
    };

  };

};

#endif // __UI_Style_FabricStyle__
