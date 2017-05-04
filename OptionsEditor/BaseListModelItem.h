//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_LIST_MODEL_ITEM__
#define __UI_BASE_LIST_MODEL_ITEM__

#include <QSettings>
#include "BaseOptionsEditor.h"
#include "BaseOptionsModelItem.h"

namespace FabricUI {
namespace OptionsEditor {

class BaseListModelItem : public BaseOptionsModelItem 
{
  /**
    BaseListModelItem is a list of BaseOptionsModelItem stored
    as a key-value pair dictionary [key(String), value(BaseOptionsModelItem)].
  */  
  Q_OBJECT
  
  public:
    /// Constructs a model item and adds it as its child.
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    /// \param settings Pointor to the settings.
    BaseListModelItem(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor=0,
      void *options=0,
      QSettings *settings=0
      );

    virtual ~BaseListModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    virtual int getNumChildren();

    /// Implementation of ValueEditor::BaseModelItem
    virtual ValueEditor::BaseModelItem* getChild(
      FTL::StrRef childName, 
      bool doCreate
      );

    /// Implementation of ValueEditor::BaseModelItem
    virtual ValueEditor::BaseModelItem* getChild(
      int index, 
      bool doCreate
      );
    
    /// Implementation of ValueEditor::BaseModelItem
    virtual void resetToDefault();

  protected:
    /// Constructs a model item and adds it as its child.
    /// \param name Name of the child item.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Child item's options.
    /// \param settings Pointor to the settings.
    void constructModel(
      const std::string &name,
      BaseOptionsEditor *editor,
      void *options=0,
      QSettings *settings=0
      );

    /// Lists of the keys.
    std::vector<std::string> m_keys;
    /// Dictionary [key, value]
    std::map<std::string, ValueEditor::BaseModelItem*> m_children;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_LIST_MODEL_ITEM__
