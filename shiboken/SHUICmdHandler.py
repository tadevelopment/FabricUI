import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, Style, Viewports
from PySide import QtCore, QtGui, QtOpenGL
from UICmdHandler import UICmdHandler


class SHUICmdHandler(UICmdHandler):

    def __init__(self, scene, scriptEditor):
        self.shGLScene = scene
        super(SHUICmdHandler, self).__init__(self.shGLScene.getClient(), scriptEditor)
        self.qUndoStack = scriptEditor.eval_globals['binding'].qUndoStack

        
    # Adds and executes a command
    def addCommand(self, cmd):
    	return False;