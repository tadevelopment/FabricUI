//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SCENEHUB_SGOBJECTPROPERTYMODELITEM_H__
#define __UI_SCENEHUB_SGOBJECTPROPERTYMODELITEM_H__

#include <FTL/StrRef.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

namespace FabricUI {
namespace SceneHub {
 
class SGObjectPropertyModelItem : public ValueEditor::BaseModelItem {

  /**
    SGObjectPropertyModelItem spcialized the ModelItems::BaseModelItem to edit 
    the values of a SGObjectProperty.
  */

  Q_OBJECT
 
  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client
    /// \param rtVal the SGObject to edit.
    /// \param isRootItem True if the prperty is the  editor root item.
    SGObjectPropertyModelItem(
      FabricCore::Client client,
      FabricCore::RTVal rtVal,
      bool isRootItem);

    /// Destructor.
    virtual ~SGObjectPropertyModelItem();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual BaseModelItem *createChild(FTL::CStrRef name);

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual int getNumChildren();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual FTL::CStrRef getChildName(int i);

    // Detects changes for this SGObjectProperty and updates the value.
    void updateFromScene();

    /// Gets the current SGObjectProperty.
    const FabricCore::RTVal& getSGObjectProperty();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual FTL::CStrRef getName();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual bool canRename();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void rename(FTL::CStrRef newName);

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void onRenamed(FTL::CStrRef oldName, FTL::CStrRef newName);
 
    virtual FTL::CStrRef getRTValType();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual QVariant getValue();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual bool hasDefault();
    
    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void resetToDefault();
 
     /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual FabricUI::ValueEditor::ItemMetadata* getMetadata();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void setMetadataImp(
      const char* key, 
      const char* value, 
      bool canUndo);

  signals: 
    /// Emitted when the value of the current SGObjectProeprty changed.
    /// Called to synchronize the KL and Qt stack, cf :`SceneHub::SHCmdHandler`.                
    void synchronizeCommands();


  protected:
    /// Implementation of :`ValueEditor::BaseModelItem`.
    /// Sets the values of the property, create a SGSetPropertyCmd command.
    virtual void setValue(
      QVariant var,
      bool commit,
      QVariant valueAtInteractionBegin);

    FabricCore::Client m_client;
    FabricCore::RTVal m_rtVal;
    std::string m_name;
    std::string m_rtValType;
    int m_lastValueVersion;
    bool m_rootItem;
};
    
} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_SGOBJECTPROPERTYMODELITEM_H__

