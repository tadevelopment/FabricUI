import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from PySide import QtCore, QtGui, QtOpenGL
from SceneHub.SceneHubWindow import SceneHubWindow
from canvas import FabricStyle

app = QtGui.QApplication([])
app.setOrganizationName('Fabric Software Inc')
app.setApplicationName('Fabric SceneHub Standalone')
app.setApplicationVersion('2.0.0')
app.setStyle( FabricStyle() )

fabricDir = os.environ.get('FABRIC_DIR', None)
if fabricDir:
    logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
    app.setWindowIcon(QtGui.QIcon(logoPath))

opt_parser = optparse.OptionParser(usage='Usage: %prog [options] [graph]')
opt_parser.add_option('-u', '--unguarded',
                      action='store_true',
                      dest='unguarded',
                      help='compile KL code in unguarded mode')

opt_parser.add_option('-n', '--noopt',
                      action='store_true',
                      dest='noopt',
                      help='compile KL code in noopt mode')

opt_parser.add_option('-e', '--exec',
                      action='store',
                      dest='script',
                      help='execute Python script on startup')
 
opt_parser.add_option('-s', '--scene',
                      action='store',
                      dest='scene',
                      type='str',
                      default='',
                      help='execute a scene.kl on startup')

opt_parser.add_option('-g', '--graph',
                      action='store',
                      dest='graph',
                      type='str',
                      default='',
                      help='execute a graph.canvas on startup')

(opts, args) = opt_parser.parse_args()

unguarded = opts.unguarded is True
noopt = opts.noopt is True
scene = opts.scene
graph = opts.graph

settings = QtCore.QSettings()
settings.setValue("mainWindow/lastPresetFolder", str("."))

qglFormat = QtOpenGL.QGLFormat()
qglContext = Viewports.RTRGLContext( qglFormat )

sceneHubWin = SceneHubWindow(settings, unguarded, noopt, scene, graph)
sceneHubWin.show()

if opts.script:
  with open(opts.script, "r") as f:
    sceneHubWin.scriptEditor.exec_(f.read())

app.exec_()
