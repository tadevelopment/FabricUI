// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_DetailsWidget__
#define __UI_DFG_TabSearch_DetailsWidget__

#include "Data.h"
#include "ItemView.h"

#include <QFrame>

class QTextEdit;

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      // TODO : is quite generic; we might want to move it elsewhere
      class Toggle : public QFrame
      {
        Q_OBJECT

        typedef QFrame Parent;

      public:
        Toggle( bool toggled = false );
        inline bool isToggled() const { return m_toggled; }

      public slots:
        virtual void setToggled( bool toggled );

      signals:
        void toggled( bool toggled );

      protected slots:
        virtual void setHovered( bool h );

      protected:
        void mouseReleaseEvent( QMouseEvent * ) FTL_OVERRIDE;
        void enterEvent( QEvent * ) FTL_OVERRIDE;
        void leaveEvent( QEvent * ) FTL_OVERRIDE;

      private:
        bool m_hovered, m_toggled;
      };

      class DetailsWidget : public QWidget
      {
        Q_OBJECT

      public:
        DetailsWidget( FabricCore::DFGHost* );
        const Result& getPreset() const;
        inline bool isEmpty() const { return m_preset == ""; }

      public slots:
        void setPreset( const TabSearch::Result& preset );
        void clear();

      signals:
        // Emitted when a Tag has been requested
        void tagRequested( const Query::Tag& tag );

      protected slots:
        void updateSize();

      private:
        FabricCore::DFGHost* m_host;
        Result m_preset;
        Label* m_name;
        typedef QTextEdit Description;
        Description* m_description;
        class PortsView;
        typedef std::map<std::string, PortsView*> PortsViews;
        PortsViews m_portsTables;
        class TagContainer;
        TagContainer* m_tagContainer;
        class Section;
        std::vector<Section*> m_sections;
        void addSection( Section* );
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_DetailsWidget__
