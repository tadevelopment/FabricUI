//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SIMPLE_MODEL__
#define __UI_BASE_SIMPLE_MODEL__

#include "BaseOptionsEditor.h"
#include "BaseOptionsModelItem.h"

namespace FabricUI {
namespace OptionsEditor {

class BaseSimpleModelItem : public BaseOptionsModelItem 
{
  /**
    BaseSimpleModelItem specializes BaseOptionsModelItem
    and defines a model for a single option (leaf).
  */  
  Q_OBJECT
  
  public:
    /// Constructs a BaseSimpleModelItem.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    /// \param settings Pointor to the settings.
    BaseSimpleModelItem(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor=0,
      void *options=0,
      QSettings *settings=0
      );

    virtual ~BaseSimpleModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    /// To override.
    virtual QVariant getValue();

    /// Implementation of ValueEditor::BaseModelItem
    /// To override.
    virtual void setValue(
      QVariant value,
      bool commit,
      QVariant valueAtInteractionBegin
      );

    /// Implementation of ValueEditor::BaseModelItem
    /// To override.
    virtual void resetToDefault();

  signals:
    /// Emitted when the value of one option has changed.
    void updated(
      );
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_SIMPLE_MODEL__
