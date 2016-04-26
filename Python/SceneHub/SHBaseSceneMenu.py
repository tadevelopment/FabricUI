
from FabricEngine.Canvas.BaseMenu import BaseMenu
    
class SHBaseSceneMenu(BaseMenu):

    """SHBaseSceneMenu

    SHBaseSceneMenu specializes BaseMenu.
    It owns a reference to a SceneHub.SHGLScene scene to edit.

    Arguments:
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
        title (String): The menu title, can be None.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shGLScene, title = None, parent = None):
        self.shGLScene = shGLScene
        super(SHBaseSceneMenu, self).__init__(title, parent)
    
    def onActiveSceneChanged(self, shGLScene):
        """Updates the current scene menu.
           Can be used as a public slot.
        """

        self.shGLScene = shGLScene
 
