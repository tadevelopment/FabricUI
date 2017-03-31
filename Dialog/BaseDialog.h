// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BaseDialog__
#define __UI_BaseDialog__

#include <QDialog>
#include <QVBoxLayout>

namespace FabricUI
{
  namespace Dialog
  {

    class BaseDialog : public QDialog 
    {
      Q_OBJECT

    public:

      BaseDialog(QWidget * parent);
      virtual ~BaseDialog();      

      virtual QVBoxLayout * inputsLayout();

    protected:

      QVBoxLayout * m_inputsLayout;

    };

  };

};


#endif // __UI_BaseDialog__
