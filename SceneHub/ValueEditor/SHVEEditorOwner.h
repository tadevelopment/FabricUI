//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGVEEditorOwner.h>

namespace FabricUI {

  namespace SceneHub
  { 
    class SHVEEditorOwner : public DFG::DFGVEEditorOwner
    {
      Q_OBJECT

    public:

      SHVEEditorOwner(DFG::DFGWidget * dfgWidget);
      ~SHVEEditorOwner();

      virtual void initConnections();

    // protected slots:

    // private:

      // void setModelRoot(
      //   FabricUI::DFG::DFGController *dfgController,
      //   FabricUI::ModelItems::BindingModelItem *bindingModelItem
      //   );

      // QSharedPointer<DFG::DFGNotifier> m_notifier;
    };
  }
}
