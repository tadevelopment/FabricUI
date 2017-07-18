import platform
import pytest
import os
from PySide import QtCore, QtGui
from PySide.QtTest import QTest
from FabricEngine import Core
from FabricEngine.FabricUI import Application
from FabricEngine.Canvas.CanvasWindow import CanvasWindow

# [andrew 20160330] FE-6364
pytestmark = pytest.mark.skipif(
        (platform.system() == 'Linux' and not os.environ.has_key('DISPLAY')) or \
            platform.system() == 'Darwin',
        reason = "missing display")

@pytest.yield_fixture(scope="module")
def canvas_app():
    app = Application.FabricApplication()
    app.setOrganizationName('Fabric Software Inc')
    app.setApplicationName('Fabric Canvas Standalone')
    yield app

@pytest.yield_fixture(scope="module")
def canvas_win(canvas_app):

    settings = QtCore.QSettings()
    unguarded = False
    noopt = True
    main_win = CanvasWindow(settings, unguarded, noopt)
    main_win.show()

    # https://doc.qt.io/qt-4.8/qttest-module.html
    QTest.qWaitForWindowShown(main_win)

    yield main_win

def test_fe5730(canvas_win):
    dfg_controller = canvas_win.dfgWidget.getDFGController()
    binding = dfg_controller.getBinding()
    root_exec = binding.getExec()
    i_name = root_exec.addExecPort('i', canvas_win.client.DFG.PortTypes.In)
    binding.setArgValue(i_name, canvas_win.client.RT.types.UInt32(5))
    canvas_win.onNewGraph(skip_save=True)

def test_fe6338(canvas_win):
    dfg_controller = canvas_win.dfgWidget.getDFGController()
    binding = dfg_controller.getBinding()
    canvas_win.dfguiCommandHandler.dfgDoSetArgValue(binding, 'not_exist',
            canvas_win.client.RT.types.UInt32(5))
    canvas_win.onNewGraph(skip_save=True)

def test_fe7013(canvas_win):
    path = os.environ['FABRIC_SCENE_GRAPH_DIR']
    path = path + '/Native/FabricUI/Test/'
    canvas_win.loadGraph(path + 'fe7013_PrimeNumbers.canvas')
    canvas_win.loadGraph(path + 'fe7013_KochCurve.canvas')
