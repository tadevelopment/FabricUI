
from FabricEngine.Canvas.HelpWidget import HelpWidget
from PySide import QtCore, QtGui

class SHHelpWidget(HelpWidget):

    """SHHelpWidget

    Simple specialization of SHHelpWidget to allow closing the owner dialog with its 'close' button
    """

    closeSignal = QtCore.Signal()

    def __init__(self, usagetFilePath, parent = None, width = 800, height = 500):
        super(SHHelpWidget, self).__init__(usagetFilePath, parent, width, height)

    def closeEvent(self, event):
        self.closeSignal.emit()
