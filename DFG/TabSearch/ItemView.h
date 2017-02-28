// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ItemView__
#define __UI_DFG_TabSearch_ItemView__

#include <QWidget>
#include <FTL/Config.h>

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

        typedef QWidget Parent;

      public:
        TagView( const std::string& tagName );
        void setScore( double score );

      signals:
        void activated( const std::string& name );

      public slots:
        void setHighlighted( bool highlighted );

      protected:
        void enterEvent( QEvent * ) FTL_OVERRIDE;
        void leaveEvent( QEvent * ) FTL_OVERRIDE;

      private slots:
        void onActivated();

      private:
        void updateHighlightColor();
        std::string m_name;
        QPushButton* m_button;
        bool m_hovered;
        bool m_highlighted;
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
