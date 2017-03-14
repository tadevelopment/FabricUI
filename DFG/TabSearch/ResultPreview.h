// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultPreview__
#define __UI_DFG_TabSearch_ResultPreview__

#include <QWidget>

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
      class ResultPreview : public QWidget
      {
        Q_OBJECT

      public:
        ResultPreview( const std::string& preset, FabricCore::DFGHost* host );
        const std::string& getPreset() const;

      signals:
        // Emitted when a Tag has been requested
        void tagRequested( const std::string& tag );

      private:
        std::string m_preset;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultPreview__
