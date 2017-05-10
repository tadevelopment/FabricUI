//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_MODEL_ITEM__
#define __UI_RTVAL_MODEL_ITEM__

#include "RTValItem.h"
#include "BaseSimpleModelItem.h"

namespace FabricUI {
namespace OptionsEditor {

class RTValModelItem 
  : public BaseSimpleModelItem
  , public FabricUI::OptionsEditor::RTValItem
{
  /**
    RTValModelItem specializes BaseSimpleModelItem 
    for RTVal of basic type (leaf).
  */    
  Q_OBJECT
  
  Q_INTERFACES(FabricUI::OptionsEditor::RTValItem)

  public:
    /// Constructs a RTValModelItem.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    /// \param settings Pointor to the settings.
    RTValModelItem(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor,
      void *options,
      QSettings *settings=0
      );

    virtual ~RTValModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    virtual QVariant getValue();
  
    /// Implementation of ValueEditor::BaseModelItem
    virtual void setValue(
      QVariant value,
      bool commit = false,
      QVariant valueAtInteractionBegin = QVariant()
      );

    /// Implementation of ValueEditor::BaseModelItem
    virtual void resetToDefault();

    /// Implementation of RTValItem.
    virtual FabricCore::RTVal getRTValOptions();

    /// Implementation of RTValItem.
    virtual void setRTValOptions(
      FabricCore::RTVal options
      );

  protected:
    /// Current value
    FabricCore::RTVal m_options; 
    /// Current value
    FabricCore::RTVal m_originalOptions; 
    /// Pointor to the settings.
    QSettings *m_settings;

  private:  
    /// \internal
    FabricCore::Client m_client;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_MODEL_ITEM__
