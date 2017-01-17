// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_LineNumberWidget__
#define __UI_KLEditor_LineNumberWidget__

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QWidget>
#include "EditorConfig.h"

namespace FabricUI
{

  namespace KLEditor
  {

    class LineNumberWidget : public QWidget
    {
      Q_OBJECT
    
    public:
    
      LineNumberWidget(QWidget * parent, const EditorConfig & config = EditorConfig());
      virtual ~LineNumberWidget();

      unsigned int lineOffset() const;

      virtual void paintEvent(QPaintEvent * event);

    public slots:
      void setLineOffset(unsigned int offset);

    private:
      EditorConfig m_config;
      QFontMetrics * m_metrics;
      unsigned int m_lineOffset;
    };

  };

};

#endif // __UI_KLEditor_LineNumberWidget__
