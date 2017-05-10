//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_MODEL_ITEM__
#define __UI_BASE_MODEL_ITEM__

#include <string>
#include <FabricUI/ValueEditor/BaseModelItem.h>

namespace FabricUI {
namespace OptionsEditor {

class BaseOptionsModelItem : public ValueEditor::BaseModelItem 
{
  /**
    BaseOptionsModelItem is the base class for any options item.
  */  
  Q_OBJECT
  
  public:
    /// Constructs a model item.
    /// \param name Name of the item.
    /// \param path Path of the item.
    BaseOptionsModelItem(
      const std::string &name,
      const std::string &path
      );

    virtual ~BaseOptionsModelItem();
 
    /// Implementation of BaseModelItem
    virtual bool hasDefault();

    /// Implementation of BaseModelItem
    virtual FTL::CStrRef getName();

    /// Get the path of the item (./.../item/child/...)
    FTL::CStrRef getPath();

  protected:
    /// Name of the item.
    std::string m_name;
    /// Path of the item.
    std::string m_path;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_MODEL_ITEM__
