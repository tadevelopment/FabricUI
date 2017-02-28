// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ItemView__
#define __UI_DFG_TabSearch_ItemView__

#include <QWidget>

class QLabel;
class QPushButton;

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
        void activated( const std::string& name );

      public slots:
        void setHighlighted( bool highlighted );

      private slots:
        void onActivated();

      private:
        std::string m_name;
        QPushButton* m_button;
      };

      class PresetView : public QWidget
      {
        Q_OBJECT

      public:
        PresetView( const std::string& presetName );
        void setScore( double score );

      public slots:
        void setHighlighted( bool highlighted );

      private:
        QLabel* m_scoreLabel;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ItemView__
