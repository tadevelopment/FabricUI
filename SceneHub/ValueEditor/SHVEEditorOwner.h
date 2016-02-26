//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ValueEditor/VEEditorOwner.h>

namespace FabricUI {

  namespace SceneHub
  { 
    class SHVEEditorOwner : public ValueEditor::VEEditorOwner
    {
      Q_OBJECT

    public:

      SHVEEditorOwner();
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
