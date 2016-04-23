import argparse, os, sys
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


class Parser(argparse.ArgumentParser):
  def error(self, message):
    sys.stderr.write('error %s\n' %  message)
    self.print_help()
    sys.exit(2)

parser = Parser()


parser.add_argument('-u', '--unguarded',
                    action='store_true',
                    help='compile KL code in unguarded mode')

parser.add_argument('-n', '--noopt',
                    action='store_true',
                    help='compile KL code wihout brackground optimization')

parser.add_argument('-d', '--documemtation',
                    action='store_true',
                    help='show the documentation and quit')

parser.add_argument('-e', '--script',
                    type=str,
                    default='',
                    help='execute Python script on startup')

parser.add_argument('-s', '--scene',
                    type=str,
                    default='',
                    help='execute a scene.kl on startup')

parser.add_argument('-g', '--graph',
                    type=str,
                    default='',
                    help='execute a graph.canvas on startup')

parser.add_argument('-m', '--multisampling',
                    type=int,
                    default=2,
                    choices=[1, 2, 4, 8],
                    help='initial multisampling on startup')

args = parser.parse_args()

unguarded = args.unguarded is True
noopt = args.noopt is True
doc = args.documemtation is True
scene = args.scene
graph = args.graph
samples = args.multisampling
script = args.script

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

if script:
  with open(script, "r") as f:
    sceneHubWin.scriptEditor.exec_(f.read())

app.exec_()
