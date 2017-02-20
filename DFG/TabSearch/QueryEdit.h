// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_QueryEdit__
#define __UI_DFG_TabSearch_QueryEdit__

#include <QLineEdit>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class QueryEdit : public QLineEdit
      {
        Q_OBJECT

      public:
        QueryEdit();

      signals:
        void queryChanged( QString query );
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_QueryEdit__
