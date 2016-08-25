/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_STATES_H__
#define __UI_SCENEHUB_STATES_H__

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <FabricCore.h>
#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI {
namespace SceneHub {
    
class SHStates : public QObject {

  /**
    SHStates is a helper class wrapping :kl-ref:`SHStates`.
    It allows easy and safe access in C++ to the KL methods.

    When possible, the logic is to write most of the code in :kl-ref:`SHStates`.
  */
    
  Q_OBJECT

  public:
    SHStates(FabricCore::Client client);
    
    /// Gets the client.
    FabricCore::Client getClient();

    /// Returns an array of SGObject that are currently in the 'selection' set. 
    /// This is used for reflecting the 3D selection to the TreeView (which might or not have these as UI nodes).
    FabricCore::RTVal getSelectedObjects();

    /// true if the inspected item (double-clicked) is a SGObject
    bool isInspectingSGObject();

    /// true if the inspected item (double-clicked) is a SGObjectProperty
    bool isInspectingSGObjectProperty();

    /// true if the inspected item (double-clicked) is a SGCanvasOperator
    bool isInspectingSGCanvasOperator();

    /// Returns the SGObject associated with the inspected item
    /// Might be valid even if !isInspectingSGObject() (eg: property's owner object is inspected)
    FabricCore::RTVal getInspectedSGObject();

    /// Returns the SGObjectProperty associated with the inspected item
    /// Might be valid even if !isInspectingSGObjectProperty() (eg: property's generator is inspected)
    FabricCore::RTVal getInspectedSGObjectProperty();

    /// Returns the SGCanvasOperator associated with the inspected item
    /// Might be valid even if !isInspectingSGCanvasOperator() (eg: generator writing to inspected property)
    FabricCore::RTVal getInspectedSGCanvasOperator();

    /// Returns a Ref to SHStates.options
    FabricCore::RTVal getOptionsRef();

    /// Updates SceneHub from the values of SHStates.options
    void updateFromOptions();

    /// Returns the active SHBaseScene (which can be driven by the TreeView)
    SHGLScene* getActiveScene();

    /// Clears the selection
    void clearSelection();

    /// Adds a SGObject to the selection
    void addSGObjectToSelection(FabricCore::RTVal sgObject);

    /// Removes a SGObject to the selection
    void removeSGObjectFromSelection(FabricCore::RTVal sgObject);

    /// Adds a SGObjectProperty to the selection
    void addSGObjectPropertyToSelection(FabricCore::RTVal sgObject);

    /// Removes a SGObjectProperty to the selection
    void removeSGObjectPropertyFromSelection(FabricCore::RTVal sgObject);

    /// Adds a SGObjectProperty generator to the selection
    void addSGObjectPropertyGeneratorToSelection(FabricCore::RTVal sgObject);
    
    /// Removes a SGObjectProperty generator to the selection
    void removeSGObjectPropertyGeneratorFromSelection(FabricCore::RTVal sgObject);


  signals:
    void sceneHierarchyChanged() const;

    void sceneChanged() const;

    void selectionChanged() const;

    void inspectedChanged() const;

    void activeSceneChanged() const;


  public slots:
    /// This should be called when the state of selection or scene might have changed
    void onStateChanged();

    void onInspectSelectedSGObject();

    void onInspectedSGObject(FabricCore::RTVal sgObject);

    void onInspectedSGObjectProperty(FabricCore::RTVal sgObjectProperty);

    void onInspectedSGObjectPropertyGenerator(FabricCore::RTVal sgObjectProperty);

    void onActiveSceneChanged(FabricUI::SceneHub::SHGLScene* scene);

    void onFrameChanged(int frame);


  private:
    /// \internal
    FabricCore::Client m_client;    
    /// \internal
    FabricCore::RTVal m_shStateVal;
    /// \internal
    SHGLScene* m_activeSHGLScene;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_STATES_H__
