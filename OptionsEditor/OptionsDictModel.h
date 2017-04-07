//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_OPTION_DICT_MODEL__
#define __UI_BASE_OPTION_DICT_MODEL__

#include <string>
#include <QSettings>
#include <FabricCore.h>
#include "BaseOptionsEditor.h"
#include <FabricUI/ValueEditor/BaseModelItem.h>

namespace FabricUI {
namespace Bases {

// Models a dictionary of Options : RTVal[String]
class OptionsDictModel : public ValueEditor::BaseModelItem {

  public:
    OptionsDictModel(
      const std::string name,
      FabricCore::RTVal dict,
      QSettings* settings,
      const std::string namePath,
      BaseOptionsEditor& editor
      );

    virtual ~OptionsDictModel();

    virtual FTL::CStrRef getName();

    virtual int getNumChildren();

    virtual BaseModelItem* getChild(
      FTL::StrRef childName, 
      bool doCreate
      );

    virtual BaseModelItem* getChild(
      int index, 
      bool doCreate
      );
    
    virtual bool hasDefault();
    
    virtual void resetToDefault();

  protected:
    const std::string m_name;
    const std::string m_namePath;
    std::map<std::string, BaseModelItem*> m_children;
    std::vector<std::string> m_keys;
};

} // namespace Bases
} // namespace FabricUI

#endif // __UI_BASE_OPTION_DICT_MODEL__
