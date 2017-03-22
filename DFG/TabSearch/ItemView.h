// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ItemView__
#define __UI_DFG_TabSearch_ItemView__

#include <QWidget>
#include <QFrame>
#include <FTL/Config.h>

#include "Data.h"

class QLabel;
class QPushButton;

inline std::string ToStdString( const QString& s )
{
  const QByteArray utf8 = s.toUtf8();
  return std::string( utf8.data(), utf8.size() );
}
inline QString ToQString( const std::string& s ) { return QString::fromUtf8( s.data(), s.size() ); }

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class HeatBar;

      void SetWidgetHighlight( QWidget* w, const bool highlighted );

      class TagView : public QWidget
      {
        Q_OBJECT

        typedef QWidget Parent;

      public:
        TagView( const Query::Tag& tag );
        void setScore( double score );

      static std::string DisplayName( const Query::Tag& result );

      signals:
        void activated( const Query::Tag& tag );

      public slots:
        void setHighlighted( bool highlighted );

      private slots:
        void onActivated();

      private:
        Query::Tag m_tag;
        QPushButton* m_button;
        bool m_hovered;
        bool m_highlighted;
      };

      class TagArrow : public QFrame
      {
        Q_OBJECT
        Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor DESIGNABLE true)
        Q_PROPERTY(QColor highlightBgColor READ highlightBgColor WRITE setHighlightBgColor DESIGNABLE true)
        Q_PROPERTY(int spacing READ spacing WRITE setSpacing DESIGNABLE true)

        typedef QFrame Parent;
        
      public:

        TagArrow();

        inline const QColor bgColor() const { return m_bgColor; }
        inline void setBgColor( const QColor& c ) { m_bgColor = c; update(); }
        inline const QColor highlightBgColor() const { return m_bgColor; }
        inline void setHighlightBgColor( const QColor& c ) { m_highlightBgColor = c; update(); }
        inline int spacing() const { return m_spacing; }
        inline void setSpacing( int s ) { m_spacing = s; update(); }

        void paintEvent( QPaintEvent * ) FTL_OVERRIDE;

        inline void setLeftHighlighted( bool hl ) { m_leftHighlighted = hl; update(); }
        inline void setRightHighlighted( bool hl ) { m_rightHighlighted = hl; update(); }
        inline void setHasRight( bool hr ) { m_hasRight = hr; update(); }

      private:
        int m_spacing;
        bool m_leftHighlighted, m_hasRight, m_rightHighlighted;
        QColor m_bgColor, m_highlightBgColor;
      };

      class PresetView : public QWidget
      {
        Q_OBJECT

      public:
        PresetView(
          const std::string& presetName,
          const std::vector<Query::Tag>& tags = std::vector<Query::Tag>()
        );
        void setScore( double score, double minScore = 0, double maxScore = 1 );

      static std::string DisplayName( const std::string& result );

      signals:
        void requestTag( const Query::Tag& tagName );

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
