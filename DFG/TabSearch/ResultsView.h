// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultsView__
#define __UI_DFG_TabSearch_ResultsView__

#include <QListView>
#include <QStringListModel>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class ResultsView : public QListView
      {
        Q_OBJECT

      public:
        ResultsView();

        QString getSelectedPreset();
        inline int numberResults() const { return m_model.rowCount(); }

      public slots:
        void setResults( std::vector<std::string> results );
        void moveSelection( int increment = +1 );

      protected:
        void setSelection( unsigned int index );

      private:
        QStringListModel m_model;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultsView__
