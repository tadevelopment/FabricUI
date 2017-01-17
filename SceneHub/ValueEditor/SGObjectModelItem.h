//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SCENEHUB_SGOBJECTMODELITEM_H__
#define __UI_SCENEHUB_SGOBJECTMODELITEM_H__

#include <FabricUI/ModelItems/RootModelItem.h>


namespace FabricUI {
namespace SceneHub {


class SGObjectModelItem : public ModelItems::RootModelItem {

  /**
    SGObjectModelItem specializes the ModelItems::RootModelItem 
    to edit the propertis of a SGObject.
  */

  Q_OBJECT
 
  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param rtVal the SGObject to edit.
    SGObjectModelItem(FabricCore::Client client, FabricCore::RTVal rtVal);

    /// Destructor.
    virtual ~SGObjectModelItem();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual BaseModelItem *createChild(FTL::StrRef name);

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual int getNumChildren();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual FTL::CStrRef getChildName(int i);

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void onStructureChanged();

    /// Gets the current edited SGObject.
    const FabricCore::RTVal& getSGObject();

    // Detects potential scene changes for this SGObject.
    // If structureChanged, this object must be recreated
    // (incremental update is not supported right now). 
    // Otherwise, property values will be updated if required.
    void updateFromScene(
      const FabricCore::RTVal& newSGObject, 
      bool& isValid, 
      bool& structureChanged);
 
    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual FTL::CStrRef getName();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual bool canRename();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void rename(FTL::CStrRef newName);

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void onRenamed(FTL::CStrRef oldName, FTL::CStrRef newName);

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual QVariant getValue();

    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual FabricUI::ValueEditor::ItemMetadata* getMetadata();
    
    /// Implementation of :`ValueEditor::BaseModelItem`.
    virtual void setMetadataImp(
      const char* key, 
      const char* value, 
      bool canUndo);


  public slots:
    /// Synchronizes the commands.
    /// Shall be emitted when a SGObjectProperty changed.
    void onSynchronizeCommands();


  signals:
    /// Emitted when a SGObjectProperty of the current SGObject is inspected.
    void propertyItemInserted(FabricUI::ValueEditor::BaseModelItem * item);

    /// Synchronizes the commands.
    /// Dumb slot to emit the synchronizeCommands signal.
    void synchronizeCommands();


  protected:
    /// Sets the values of the SGObject.
    /// Does nothing.
    virtual void setValue(
      QVariant var,
      bool commit,
      QVariant valueAtInteractionBegin);

    /// Safe call to get the SGObjectProperty of the current SGObject. 
    void ensurePropertiesRTVal();

    FabricCore::Client m_client;
    FabricCore::RTVal m_rtVal;

    FabricCore::RTVal m_lastStructureVersionRtVal;
    FabricCore::RTVal m_lastValuesVersionRtVal;
    FabricCore::RTVal m_isValidRtVal;
    FabricCore::RTVal m_structureChangedRtVal;
    FabricCore::RTVal m_valueChangedRtVal;

    std::string m_name;
    FabricCore::RTVal m_propertiesRtVal;
    std::map<std::string, unsigned int> m_propertyNameMap;
};

} // namespace SceneHub
} // namespace FabricUI

#endif //__UI_SCENEHUB_SGOBJECTMODELITEM_H__

