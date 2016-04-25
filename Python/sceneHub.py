#!/usr/bin/env python

"""SceneHub launcher script."""

import argparse, os, sys

if sys.version_info < (2, 7):
    raise Exception('canvas.py currently requires Python 2.7')

from PySide import QtCore, QtGui
from FabricEngine.SceneHub.SceneHubWindow import SceneHubWindow
from FabricEngine.Canvas.FabricStyle import FabricStyle
from FabricEngine.Canvas.FabricParser import FabricParser
from FabricEngine.Canvas.FabricParser import CheckExtension

if __name__ == "__main__":
    # This only runs when launched directly from the command line.
    # A QApplication is setup and the SceneHub Window is instanced and attached to
    # the QApplication and shown.
     
    app = QtGui.QApplication([])
    app.setOrganizationName('Fabric Software Inc')
    app.setApplicationName('Fabric SceneHub Standalone')
    app.setApplicationVersion('2.0.0')
    app.setStyle( FabricStyle() )

    fabricDir = os.environ.get('FABRIC_DIR', None)
    if fabricDir:
        logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
        app.setWindowIcon(QtGui.QIcon(logoPath))
     
    parser = Parser()

    parser.add_argument('scene',
                        nargs='?',
                        action=CheckExtension({'kl'}),
                        help='SceneHub scene to load')

    parser.add_argument('-g', '--graph',
                        type=str,
                        default='',
                        action=CheckExtension({'canvas'}),
                        help='execute a graph.canvas on startup')

    parser.add_argument('-u', '--unguarded',
                        action='store_true',
                        help='compile KL code in unguarded mode')

    parser.add_argument('-n', '--noopt',
                        action='store_true',
                        help='compile KL code wihout brackground optimization')

    parser.add_argument('-e', '--exec',
                        action='store',
                        dest='exec_',
                        help='Python code to execute on startup')

    parser.add_argument('-s', '--script',
                        action='store',
                        dest='script',
                        help='Python script file to execute on startup')

    parser.add_argument('-d', '--documemtation',
                        action='store_true',
                        help='show the documentation and quit')

    parser.add_argument('-m', '--multisampling',
                        type=int,
                        default=2,
                        choices=[1, 2, 4, 8],
                        help='initial multisampling on startup')

    args = parser.parse_args()

    unguarded = args.unguarded is True
    noopt = args.noopt is True
    samples = args.multisampling
    script = args.script

    #Check the options
    if samples is not 1 and samples is not 2 and samples is not 4 and samples is not 8:
        print "Unsuported samples number :" + str(samples) + " , use (1|2|4|8) "
        sys.exit()
 
    #Display the documentation if asked
    if args.documemtation is True:
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
        args.scene, 
        args.graph,
        sceneamples)

    sceneHubWin.show()

    if args.exec_:
        mainWin.scriptEditor.exec_(args.exec_)

    if args.script:
        with open(args.script, "r") as f:
            mainWin.scriptEditor.exec_(f.read())

    app.exec_()
