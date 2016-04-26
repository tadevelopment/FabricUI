/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_GLRENDERER_H__
#define __UI_SCENEHUB_GLRENDERER_H__

#include <FabricCore.h>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QDrag>
#include <QtGui/QVector3D>
#include <QtGui/QMouseEvent>
 
namespace FabricUI {
namespace SceneHub {

class SHGLRenderer {

  /**
    SHGLRenderer is a helper class wrapping :kl-ref:`SHGLRenderer`.
    It allows easy and safe access in C++ to the KL methods.

    When possible, the logic is write most of the code in :kl-ref:`SHGLRenderer`, 
    to provide app-independent example code and behavior.
  */
  
  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    SHGLRenderer(FabricCore::Client client);

    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param shRenderer A reference to the SHGLRenderer.
    SHGLRenderer( FabricCore::Client client, FabricCore::RTVal shRenderer );

    /// Updates the Renderer, if set externally
    void update();

    /// Gets the client.
    FabricCore::Client getClient() { return m_client; }

    /// \internal
    FabricCore::RTVal getSHGLRenderer() { return m_shGLRendererVal; }
        
    /// Gets the drawing statistics.
    /// \param viewportID The viewport ID.
    /// \param obj the total number of renderer objects
    /// \param point the total number of renderer points
    /// \param line the total number of renderer lines
    /// \param triangle the total number of renderer triangless
    QList<unsigned int> getDrawStats(unsigned int viewportID);
    
    /// Fast getter to the viewport at this viewportID.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getViewport(unsigned int viewportID);
    
    /// Gets/creates a viewport.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getOrAddViewport(unsigned int viewportID);
    
    /// Gets/creates a setreo-viewport.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getOrAddStereoViewport(unsigned int viewportID);
    
    /// Removes the viewport.
    /// \param viewportID The viewport ID. 
    void removeViewport(unsigned int viewportID);
    
    /// Sets the viewport camera as orthographic.
    /// \param viewportID The viewport ID.
    void setOrthographicViewport(unsigned int viewportID, bool orthographic);
    
    /// Gets the viewport camera.
    /// \param viewportID The viewport ID.
    FabricCore::RTVal getCamera(unsigned int viewportID);
    
    /// Returns a ray from a Viewport and a position in camera space.
    /// \param viewportID The ID of the viewport.
    /// \param pos The mouse's position in the viewport
    FabricCore::RTVal castRay(unsigned int viewportID, QPoint pos);

    /// This helper method will raycast in the scene. 
    /// If no result is found, it will intersect with the Y = 0 plane. 
    /// If this is outside the near/far camera range, it will return the origin.
    /// \param viewportID The ID of the viewport.
    /// \param pos The mouse's position in the viewport
    QList<float> get3DScenePosFrom2DScreenPos(unsigned int viewportID, QPoint pos);

    /// This helper method will raycast in the scene and
    /// return the closest hit SGObject (which might be invalid).
    /// \param viewportID The ID of the viewport.
    /// \param pos The mouse's position in the viewport
    FabricCore::RTVal getSGObjectFrom2DScreenPos( unsigned int viewportID, QPoint pos );

    /// Activates the playback.
    /// \param playback It true, playback.
    void setPlayback(bool playback);
    
    /// Checks if the playback is active.
    bool isPlayback();
    
    /// Renders within this viewport.
    /// \param viewportID The viewport ID.
    /// \param width the viewport width.
    /// \param height the viewport height.
    /// \param samples Anti-aliasing number of samples.
    void render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples);
    
    /// Renders within this viewport.
    /// \param viewportID The viewport ID.
    /// \param width the viewport width.
    /// \param height the viewport height.
    /// \param samples Anti-aliasing number of samples.
    /// \param drawPhase Drawing phases (pre-post draw).
    void render(unsigned int viewportID, unsigned int width, unsigned int height, unsigned int samples, unsigned int drawPhase);
    
    /// Propagates the events.
    /// \param event The event.
    /// \param redrawAllViewports It true, refresh the render.
    /// \param dragging If true when dragging an asset or texture in the scene.
    bool onEvent(unsigned int viewportID, QEvent *event, bool &redrawAllViewports, bool dragging);
    
    /// Returns a reference to the ToolDispatcher.
    FabricCore::RTVal getToolDispatcher();

    /// Gets the names and the keys of the registered tools.
    /// \param toolNames Name of the registered tools.
    /// \param enableKeys Key of the registered tools.
    QList<QStringList> getRegisteredTools();
    
    /// Returns the selection set if any.
    FabricCore::RTVal getSelectionSet();
    
    /// Checks if the selection changed from the manipulation system.
    /// Synchronizes with the tree-view.
    bool selectionChangedFromManips();
    
    /// Gets the category of the selection.
    /// Used to know what type of element is selected.
    /// For showing the right contextual menu.
    QString getSelectionCategory();


  private:
    /// \internal
    FabricCore::Client m_client;    
    /// \internal
    FabricCore::RTVal m_shGLRendererVal;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_GLRENDERER_H__
