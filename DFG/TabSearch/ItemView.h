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
      class HeatBar;
      static const char* HighlightedPropertyStr = "highlighted";

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

      private slots:
        void onActivated();

      private:
        std::string m_name;
        QPushButton* m_button;
        bool m_hovered;
        bool m_highlighted;
      };

      class PresetView : public QWidget
      {
        Q_OBJECT

      public:
        PresetView(
          const std::string& presetName,
          const std::vector<std::string>& tags = std::vector<std::string>()
        );
        void setScore( double score, double minScore = 0, double maxScore = 1 );

      signals:
        void requestTag( const std::string& tagName );

      public slots:
        void setHighlighted( bool highlighted );

      private:
        // TODO: Interaction
        std::vector<TagView*> m_tagViews;
        HeatBar* m_heatBar;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ItemView__
