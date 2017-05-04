//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_ARRAY_MODEL_ITEM__
#define __UI_RTVAL_ARRAY_MODEL_ITEM__

#include "RTValItem.h"
#include "BaseListModelItem.h"

namespace FabricUI {
namespace OptionsEditor {

class RTValArrayModelItem 
  : public BaseListModelItem
  , public FabricUI::OptionsEditor::RTValItem
{
  /**
    RTValArrayModelItem specializes BaseListModelItem 
    for RTVals arrays.
  */  
  Q_OBJECT

  Q_INTERFACES(FabricUI::OptionsEditor::RTValItem)
  
  public:
    /// Constructs a RTValArrayModelItem.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    /// \param settings Pointor to the settings.
    RTValArrayModelItem(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor,
      void *options,
      QSettings *settings=0
      );

    virtual ~RTValArrayModelItem();

    /// Implementation of BaseOptionsArrayModel.
    virtual FabricCore::RTVal getRTValOptions();

    /// Implementation of RTValItem.
    /// Throws an error if 'options' isn't a RTVal array.  
    virtual void setRTValOptions(
      FabricCore::RTVal options
      );

  private:  
    /// \internal
    FabricCore::Client m_client;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_ARRAY_MODEL_ITEM__
