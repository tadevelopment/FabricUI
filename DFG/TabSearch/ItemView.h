// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ItemView__
#define __UI_DFG_TabSearch_ItemView__

#include <QLabel>
#include <QPushButton>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class TagView : public QPushButton
      {
        Q_OBJECT

      public:
        TagView( const std::string& tagName );
        void setScore( double score );

      signals:
        void clicked( QString name );
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
