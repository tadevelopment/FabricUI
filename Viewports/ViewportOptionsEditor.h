//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_VIEWPORT_OPTION_EDITOR_H
#define __UI_VIEWPORT_OPTION_EDITOR_H

#include <FabricUI/OptionsEditor/BaseRTValOptionsEditor.h>
 
namespace FabricUI {
namespace Viewports {

class ViewportOptionsEditor 
  : public OptionsEditor::BaseRTValOptionsEditor
{
  /**
    RTValDictOptionsModelItem gathers OptionsModels in a key-value pair 
    dictionary [key(String), value(RTVal)].
  */  
  Q_OBJECT
 
  public:
    ViewportOptionsEditor( 
      FabricCore::Client client, 
      QSettings *settings
      );

    virtual ~ViewportOptionsEditor();
    
    public slots:  
      /// Implementation of BaseOptionsEditor.
      virtual void resetModel(
        void *options=0
        );
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_VIEWPORT_OPTION_EDITOR_H
