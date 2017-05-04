//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_ITEM__
#define __UI_RTVAL_ITEM__

#include <QObject>
#include <FabricCore.h>

namespace FabricUI {
namespace OptionsEditor {

class RTValItem {
  
  public:
    /// Sets the dictionary of options.
    virtual void setRTValOptions(
      FabricCore::RTVal options
      ) = 0;

    /// Gets the dictionary of options.
    virtual FabricCore::RTVal getRTValOptions() = 0;
};

} // namespace OptionsEditor 
} // namespace FabricUI

Q_DECLARE_INTERFACE(
  FabricUI::OptionsEditor::RTValItem, 
  "FabricUI.OptionsEditor.RTValItem/1.0"
  )

#endif // __UI_RTVAL_ITEM__
