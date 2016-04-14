
from PySide import QtCore, QtGui

class HelpWidget(QtGui.QWidget):

  def __init__(self, parent = None):
    super(HelpWidget, self).__init__(parent)
    
    txtEdit = QtGui.QTextEdit(self)
    txtEdit.setText("\
      SceneHub is a work-in-progress app framework for viewing and editing SceneGraph objects.\n\
      This is in an early 'alpha' state and can serve as an example or for testing.\n\
      \n\
      The following operations are supported:\n\
      \n\
      Tree view:\n\
      \n\
      - generated instances will be created as branches are expanded\n\
      - selection will be reflected in the 3D view, enabling the transform manipulator\n\
      - with an item selected, right-clicking will open a contextual menu to expand or load recursively\n\
      \n\
      3D view:\n\
      \n\
      - partially loaded assets are shown as bounding boxes (if at least one of the child isn't loaded)\n\
      \n\
      - hold 'Alt' key to navigate the camera:\n\
      \n\
        - click & drag: orbit\n\
        - middle-click & drag: pan\n\
        - wheel or right-click & drag: zoom\n\
        - arrow keys to orbit and zoom\n\
      \n\
      - click on a geometry to select it. This will activate the transform tool:\n\
      \n\
        - press S, R, T to change transform mode (might need to first click in the 3D view)\n\
        - click & drag the gizmos to change the transform\n\
      \n\
      - With a selected geometry, right-click to have a contextual menu for:\n\
      \n\
        - light: changing the color or intensity\n\
        - geometry: setting the color (shared), local color (instance specific) or texture\n\
        - parent: setting the color (propagated to children)\n\
      \n\
      - click in the background to un-select. Then, right-click to have a contextual menu for:\n\
      \n\
        - adding a geometry from a file (Fbx, Alembic)\n\
        - creating a light\n\
      \n\
      - drag & drop to the 3D view:\n\
      \n\
        - Fbx or Alembic file:\n\
          - adds the asset as a child of the 'root'\n\
          - bounding box'es bottom placed under mouse's cursor (raycast)\n\
          - assets dropped multiple times share their content in the SceneGraph (load once)\n\
          - if holding `Ctrl` key: will load recursively (expand)\n\
      \n\
        - Image file:\n\
          - set as a textured color if dropped on a geometry with UVs\n\
          - images dropped multiple times share their content in the SceneGraph (load once)\n\
          - if holding `Ctrl` key: will set as a local texture (instance specific)\n\
    ")

    txtEdit.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint)
    txtEdit.setReadOnly( True )
 
    closeButton = QtGui.QPushButton("close", self)

    vbox = QtGui.QVBoxLayout()
    vbox.addWidget(txtEdit)
    vbox.addWidget(closeButton)
    closeButton.clicked.connect(self.close)

    self.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding);

    self.resize(800, 500)
    self.setLayout(vbox)  
