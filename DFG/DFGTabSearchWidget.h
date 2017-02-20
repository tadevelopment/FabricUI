// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGTabSearchWidget__
#define __UI_DFG_DFGTabSearchWidget__

#include "DFGConfig.h"
#include <SplitSearch/SplitSearch.hpp>

#include <QWidget>
#include <QFocusEvent>

namespace FabricUI
{

  namespace DFG
  {
    class DFGWidget;

    class DFGAbstractTabSearchWidget : public QWidget
    {
      Q_OBJECT

    public:
      virtual void showForSearch( QPoint globalPos ) = 0;

    signals :
      void enabled( bool );

    };

    class DFGBaseTabSearchWidget : public DFGAbstractTabSearchWidget
    {
      Q_OBJECT

    public:

      DFGBaseTabSearchWidget(DFGWidget * parent, const DFGConfig & config);
      virtual ~DFGBaseTabSearchWidget();

      void showForSearch( QPoint globalPos );
      void showForSearch();

      virtual void mousePressEvent(QMouseEvent * event);
      virtual void mouseMoveEvent(QMouseEvent * event);
      virtual void keyPressEvent(QKeyEvent * event);
      virtual void paintEvent(QPaintEvent * event);
      virtual void hideEvent(QHideEvent * event);

    signals: 

      void enabled(bool);

    protected:

      virtual void focusOutEvent(QFocusEvent * event);
      virtual bool focusNextPrevChild(bool next);
      virtual void updateSearch();
      virtual int margin() const;

      QString resultLabel(unsigned int index) const;
      int indexFromPos(QPoint pos);
      int widthFromResults() const;
      int heightFromResults() const;
      void updateGeometry();

      char const *getHelpText() const;

      virtual unsigned int resultsSize() const = 0;
      virtual FTL::StrRef getName( unsigned int index ) const = 0;
      virtual void clear() = 0;
      virtual void select( unsigned int index ) = 0;
      virtual bool acceptKey( const Qt::Key key ) const;

      void addNodeForIndex( unsigned index );

      QPoint m_originalLocalPos;

      DFGWidget * m_parent;
      DFGConfig m_config;
      QFontMetrics m_queryMetrics;
      QFontMetrics m_resultsMetrics;
      QFontMetrics m_helpMetrics;

      int m_currentIndex;
      QString m_search;
    };

    class DFGLegacyTabSearchWidget : public DFGBaseTabSearchWidget
    {
      Q_OBJECT

    public:
      DFGLegacyTabSearchWidget( DFGWidget * parent, const DFGConfig & config )
        : DFGBaseTabSearchWidget( parent, config )
      {}

    protected:

      unsigned int resultsSize() const FTL_OVERRIDE { return m_results.getSize(); }
      FTL::StrRef getName( unsigned int index ) const FTL_OVERRIDE;
      void clear() FTL_OVERRIDE { m_results.clear(); }
      void updateSearch() FTL_OVERRIDE;
      void select( unsigned int index ) FTL_OVERRIDE { m_results.select( index ); }

    private:
      FabricServices::SplitSearch::Matches m_results;
    };

    class DFGTabSearchWidget : public DFGBaseTabSearchWidget
    {
      Q_OBJECT

    public:
      DFGTabSearchWidget( DFGWidget * parent, const DFGConfig & config );

      unsigned int resultsSize() const FTL_OVERRIDE { return m_results.size(); }
      FTL::StrRef getName( unsigned int index ) const FTL_OVERRIDE;
      void clear() FTL_OVERRIDE { m_results.clear(); }
      void updateSearch() FTL_OVERRIDE;
      void select( unsigned int index ) FTL_OVERRIDE {}
      bool acceptKey( const Qt::Key key ) const FTL_OVERRIDE;

    private:
      std::vector<std::string> m_results;
    };

  };

};

#endif // __UI_DFG_DFGTabSearchWidget__
