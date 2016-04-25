
from FabricEngine.Canvas.BaseMenu import BaseMenu
    
class SHBaseMenu(BaseMenu):

    """SHBaseMenu

    SHBaseMenu is a specialization of BaseMenu.
    It owns a reference to a SceneHub.SHGLScene scene to edit.

    Arguments:
        shGLScene (SceneHub.SHGLScene): A reference to a shGLScene.
        title (String): The menu title, can be None.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shGLScene, title = None, parent = None):
        self.shGLScene = shGLScene
        super(SHBaseMenu, self).__init__(title, parent)
    
    def onActiveSceneChanged(self, shGLScene):
        """Update the current scene menu.
           Can be used as a public slot.
        """
        self.shGLScene = shGLScene
 
