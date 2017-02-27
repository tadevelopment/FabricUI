// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ItemView__
#define __UI_DFG_TabSearch_ItemView__

#include <QWidget>

class QLabel;

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class TagView : public QWidget
      {
        Q_OBJECT

      public:
        TagView( const std::string& tagName );
        void setScore( double score );

      signals:
        void activated( QString name );

      private slots:
        void onActivated();

      private:
        QString m_name;
      };

      class PresetView : public QWidget
      {
        Q_OBJECT

      public:
        PresetView( const std::string& presetName );
        void setScore( double score );

      private:
        QLabel* m_scoreLabel;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ItemView__
