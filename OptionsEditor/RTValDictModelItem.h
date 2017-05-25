//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_DICT_MODEL_ITEM__
#define __UI_RTVAL_DICT_MODEL_ITEM__

#include "BaseRTValOptionsEditor.h"
#include "BaseRTValModelItem.h"

namespace FabricUI {
namespace OptionsEditor {

class RTValDictModelItem : public BaseRTValModelItem 
{
  /**
    RTValArrayModelItem specializes BaseRTValModelItem 
    for RTVals dictionaries.
  */  
  Q_OBJECT

  public:
    /// Constructs a RTValDictModelItem .
    /// \param name Name of the item.
    /// \param path Item's path.
    /// \param editor Pointor to the OptionsEditor.
    /// \param options Item's options.
    /// \param settings Pointor to the settings.
    RTValDictModelItem(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options,
      QSettings *settings=0
      );

    virtual ~RTValDictModelItem();

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

    /// Implementation of BaseRTValModelItem.
    virtual FabricCore::RTVal getRTValOptions();

    /// Implementation of BaseRTValModelItem.
    /// Throws an error if 'options' isn't a RTVal dict.  
    virtual void setRTValOptions(
      FabricCore::RTVal options
      );

  private:
    /// Lists of the options' key.
    FabricCore::Context m_context;
    /// Lists of the keys.
    std::vector<std::string> m_keys;
    /// Dictionary [key, value]
    std::map<std::string, BaseRTValModelItem*> m_children;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_DICT_MODEL_ITEM__