
from PySide import QtCore, QtGui

class HelpWidget(QtGui.QWidget):

  def __init__(self, parent = None):
    super(HelpWidget, self).__init__(parent)
    
    self.txtEdit = QtGui.QTextEdit(self)
    self.readfile("test.txt")
    self.txtEdit.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint)
    self.txtEdit.setReadOnly( True )
    closeButton = QtGui.QPushButton("close", self)
 
    vbox = QtGui.QVBoxLayout()
    vbox.addWidget(self.txtEdit)
    vbox.addWidget(closeButton)
    self.setLayout(vbox)  

    self.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding);
    self.resize(800, 500)

    closeButton.clicked.connect(self.close)

  def readfile(self, path):
    file_ = QtCore.QFile(path);
    if file_ is not None:
      self.txtEdit.clear();
      if file_.open(QtCore.QIODevice.ReadOnly | QtCore.QIODevice.Text):
        stream = QtCore.QTextStream(file_)
        while not stream.atEnd():
          line = stream.readLine()
          self.txtEdit.setText(self.txtEdit.toPlainText()+ line + str("\n"))  
      file_.close()
 