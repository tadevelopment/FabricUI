import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, Style, Viewports, TimeLine
from PySide import QtCore, QtGui, QtOpenGL
from sceneHubWindow import SceneHubWindow
from canvasWindow import FabricStyle

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
opt_parser.add_option('-e', '--exec',
                      action='store',
                      dest='script',
                      help='execute Python script on startup')
(opts, args) = opt_parser.parse_args()

unguarded = opts.unguarded is True

settings = QtCore.QSettings()
settings.setValue("mainWindow/lastPresetFolder", str("."))
sceneHubWin = SceneHubWindow(settings, unguarded)
sceneHubWin.show()

for arg in args:
    sceneHubWin.loadGraph(arg)

if opts.script:
    with open(opts.script, "r") as f:
        sceneHubWin.scriptEditor.exec_(f.read())

app.exec_()
