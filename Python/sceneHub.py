import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.SceneHub.SceneHubWindow import SceneHubWindow
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

opt_parser = optparse.OptionParser()


opt_parser.add_option('-u', '--unguarded',
                      action='store_true',
                      dest='unguarded',
                      help='compile KL code in unguarded mode')

opt_parser.add_option('-n', '--noopt',
                      action='store_true',
                      dest='noopt',
                      help='compile KL code wihout brackground optimization')

opt_parser.add_option('-d', '--documemtation',
                      action='store_true',
                      dest='documemtation',
                      help='show the documentation and quit')

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

opt_parser.add_option('-m', '--multisampling',
                      action='store',
                      dest='multisampling',
                      type='int',
                      default='2',
                      help='multisampling (1|2|4|8)')

(opts, args) = opt_parser.parse_args()

unguarded = opts.unguarded is True
noopt = opts.noopt is True
doc = opts.documemtation is True
scene = opts.scene
graph = opts.graph
samples = opts.multisampling

#Check the options
if samples is not 1 and samples is not 2 and samples is not 4 and samples is not 8:
  print "Unsuported samples number :" + str(samples) + " , use (1|2|4|8) "
  sys.exit()

if scene != "" and os.path.splitext(scene)[1] != ".kl":
  print str(scene) + " is an unsupported scene format, use a .kl extension file"
  sys.exit()

if graph != "" and os.path.splitext(graph)[1] != ".canvas":
  print str(graph) + " is an unsupported graph format, use a .canvas extension file"
  sys.exit()

#Display the usage if asked
if doc:
  file_ = open(os.path.expandvars('${FABRIC_DIR}/Python/2.7/FabricEngine/SceneHub/SceneHubUsage.txt'), 'r')
  print file_.read()
  sys.exit()

#Starts the app
settings = QtCore.QSettings()
settings.setValue("mainWindow/lastPresetFolder", str("."))

sceneHubWin = SceneHubWindow(
  settings, 
  unguarded, 
  noopt, 
  scene, 
  graph,
  samples)

sceneHubWin.show()

if opts.script:
  with open(opts.script, "r") as f:
    sceneHubWin.scriptEditor.exec_(f.read())

app.exec_()
