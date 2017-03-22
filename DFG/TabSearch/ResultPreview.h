// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultPreview__
#define __UI_DFG_TabSearch_ResultPreview__

#include <QFrame>
#include <FTL/Config.h>

class QLabel;

namespace FabricCore
{
  class DFGHost;
}

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

      class ResultPreview : public QFrame
      {
        Q_OBJECT

      public:
        ResultPreview( FabricCore::DFGHost* );
        const std::string& getPreset() const;
        inline bool isEmpty() const { return m_preset == ""; }

      public slots:
        void setPreset( const std::string& preset );
        void clear();

      signals:
        // Emitted when a Tag has been requested
        void tagRequested( const std::string& tag );

      protected slots:
        void updateSize();

      private:
        FabricCore::DFGHost* m_host;
        std::string m_preset;
        QLabel* m_name;
        QLabel* m_description;
        class PortsView;
        PortsView* m_portsTable;
        class TagsView;
        TagsView* m_tagsView;
        class Section;
        std::vector<Section*> m_sections;
        void addSection( Section* );
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultPreview__
