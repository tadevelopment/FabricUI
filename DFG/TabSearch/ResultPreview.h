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
        ResultPreview( QString preset, FabricCore::DFGHost* host );
        QString getPreset() const;

      signals:
        // Emitted when a Tag has been requested
        void tagRequested( QString tag );

      private:
        QString m_preset;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultPreview__
