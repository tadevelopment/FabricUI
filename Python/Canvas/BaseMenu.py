
from PySide import QtCore, QtGui


class BaseMenu(QtGui.QMenu):

    """BaseMenu

    BaseMenu is a based class for any Menu.
    It's a basic specialization of the QMenu class.

    Arguments:
        title (String): The menu title, can be None.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, title = None, parent = None):
        super(BaseMenu, self).__init__(parent)
        if title is not None : self.setTitle(title)
        self.constructMenu()

    def constructMenu(self):
        """Construct the Menu, to override.
        """
        
        raise Exception('You have to override the BaseMenu class')

