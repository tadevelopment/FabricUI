// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultPreview__
#define __UI_DFG_TabSearch_ResultPreview__

#include <QFrame>
#include <QLabel>

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

      private:
        FabricCore::DFGHost* m_host;
        std::string m_preset;
        QLabel* m_name;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultPreview__
