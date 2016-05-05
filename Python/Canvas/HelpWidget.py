
from PySide import QtCore, QtGui

class HelpWidget(QtGui.QWidget):

    """HelpWidget

    HelpWidget dispalys within a QTextEdit the application usage.
    The usage description is defined in a external file (eg .txt).

    Arguments:
        usagetFilePath (String): The path of the usage file.
        parent (QWidget): The menu widget parent, can be None.
        width (int): The widget width, 800 by default.
        height (int): The widget height, 500 by default.

    """

    def __init__(self, usagetFilePath, parent = None, width = 800, height = 500):
        super(HelpWidget, self).__init__(parent)
        
        self.txtEdit = QtGui.QTextEdit(self)
        self.__readfile(usagetFilePath)
        self.txtEdit.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint)
        self.txtEdit.setReadOnly( True )
        closeButton = QtGui.QPushButton("close", self)
     
        vbox = QtGui.QVBoxLayout()
        vbox.addWidget(self.txtEdit)
        vbox.addWidget(closeButton)
        self.setLayout(vbox)  

        self.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding);
        self.resize(width, height)

        closeButton.clicked.connect(self.close)

    def __readfile(self, path):
        file_ = QtCore.QFile(path);
        
        if file_ is not None:
            self.txtEdit.clear();
            if file_.open(QtCore.QIODevice.ReadOnly | QtCore.QIODevice.Text):
                stream = QtCore.QTextStream(file_)
                while not stream.atEnd():
                    line = stream.readLine()
                    self.txtEdit.setText(self.txtEdit.toPlainText()+ line + str("\n"))  
            file_.close()
