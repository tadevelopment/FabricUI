// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_LineEdit__
#define __UI_ValueEditor_LineEdit__

#include <QSlider>
#include <QComboBox>
#include <QLineEdit>

#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {
    class LineEdit : public QLineEdit
    {
      Q_OBJECT

    public:

      LineEdit(QWidget * parent);
      virtual ~LineEdit();

    public slots:

      virtual void onEditingFinished();
      virtual void setLineEditText(QString text);

    signals:

      void lineEditTextEdited(const QString & text);

    private:

      QString m_lastText;
    };

  };

};

#endif // __UI_ValueEditor_LineEdit__
