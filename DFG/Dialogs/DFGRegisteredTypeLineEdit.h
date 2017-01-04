// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRegisteredTypeLineEdit__
#define __UI_DFG_DFGRegisteredTypeLineEdit__

#include "DFGAutoCompleteLineEdit.h"
#include <FabricCore.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGRegisteredTypeLineEdit : public DFGAutoCompleteLineEdit
    {
      Q_OBJECT

    public:

      DFGRegisteredTypeLineEdit(QWidget * parent, FabricCore::Client & client, QString text = "");

      /// Check if the line-edit text represents a valid KL type.
      bool checkIfTypeExist();

      /// Diplays a warning pop-up informing that the type is not valid.
      void displayInvalidTypeWarning();

    public slots:
      /// Update the registerd KL type list (e.g, an extension has been loaded).
      void onUpdateRegisteredTypeList();

    private:
      /// \internal
      /// Stores the available KL types for auto-completion
      QStringList m_registerKLTypeList;
      /// \internal
      FabricCore::Client m_client;
    };

  };

};

#endif // __UI_DFG_DFGRegisteredTypeLineEdit__
