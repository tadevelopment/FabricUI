
import platform
import pytest
import os
import sys
import glob

from optparse import OptionParser

import tempfile

import png

from FabricEngine.FabricUI import Application
from FabricEngine.Canvas.CanvasWindow import CanvasWindow
from PySide import QtCore, QtGui
from PySide.QtTest import QTest

base_dir              = os.environ['FABRIC_SCENE_GRAPH_DIR']
canvas_ref_dir = os.path.join(base_dir, "Native", "FabricUI", "Test", "ScriptEditor")   
canvas_out_dir = os.path.join(base_dir, "build", os.environ['FABRIC_BUILD_OS'], os.environ['FABRIC_BUILD_ARCH'], os.environ['FABRIC_BUILD_TYPE'], "Native", "FabricUI", "Test", "ScriptEditor")
canvas_tests_dir      = os.path.join(base_dir, "Native", "FabricUI", "Test", "ScriptEditor")   


# [andrew 20160330] FE-6364
pytestmark = pytest.mark.skipif(
        (platform.system() == 'Linux' and not os.environ.has_key('DISPLAY')) or \
            platform.system() == 'Darwin',
        reason = "missing display")

        
class CanvasTestWindow(CanvasWindow):

  #test_name = ""
  test_output = ""
  
  def setTest(self, name):    
    self.test_output = ""
    
  def _reportCallback(self, source, level, line):
    if not (line.startswith("[FABRIC:MT] Loaded extension") or line.startswith("graph loaded")):
      self.test_output += line + "\n"
    super(CanvasTestWindow, self)._reportCallback(source, level, line)
        
@pytest.fixture(scope="module")
def canvas_win():
    global app
    if not QtGui.QApplication.instance():
        app = Application.FabricApplication()
        app.setOrganizationName('Fabric Software Inc')
        app.setApplicationName('Fabric Canvas Standalone')
    else:
        app = QtGui.QApplication.instance()
        
    settings = QtCore.QSettings()
    unguarded = False
    noopt = True
    main_win = CanvasTestWindow(settings, unguarded, noopt)
    main_win.show()

    # https://doc.qt.io/qt-4.8/qttest-module.html
    QTest.qWaitForWindowShown(main_win)

    return main_win

def png_imagediff(fileName1, fileName2):
  reader1 = png.Reader(filename=fileName1 + ".png")
  reader2 = png.Reader(filename=fileName2 + ".png")

  w1, h1, pixels1, metadata1 = reader1.read_flat()
  w2, h2, pixels2, metadata2 = reader2.read_flat()

  if w1 != w2 or h1 != h2 :
    print "Error, images don't have the same dimenssion"
    return -1;

  pixel_byte_width = 4 if metadata1['alpha'] else 3
  new_pixel_value = (255, 0, 0, 0) if metadata1['alpha'] else (255, 0, 0)
    
  res = 0.0;
  for i in range(0, len(pixels1)) :
    res = res + (pixels1[i] - pixels2[i]) * (pixels1[i] - pixels2[i]);
  res = res / float(len(pixels1));
  return res

def save_image(canvas_win, width, height, filepath):
  # Make sure to process all the events before setting the geometry of the viewport an drawing
  app.processEvents()
  canvas_win.viewport.setGeometry(0,0,width,height)  
  canvas_win.viewport.paintGL()
  canvas_win.viewport.swapBuffers()
  image = canvas_win.viewport.grabFrameBuffer()
  #print "there is an image with width and height",image.width(),image.height()  
  status = image.save(filepath + os.extsep + "png", "PNG")
  return status  
  
def compare_images(canvas_win, dir, test, replace_ref_image):
  test_out_images_dir = os.path.join(canvas_out_dir, dir)
  image_filepath = os.path.join(test_out_images_dir, test)
  image_ref_filepath = os.path.join(canvas_ref_dir, dir, test)
  canvas_win.viewport.setGridVisible(False, True)
  
  create_ref_image = False
  # If the _ref file doesn't exist or replace_ref_image is set to True, create a new ref image
  if (not os.path.exists(image_ref_filepath + "_ref" + os.extsep + "png")) or replace_ref_image:
    create_ref_image = True
    save_image(canvas_win, 512, 512, image_ref_filepath + "_ref")

  # If the output test dir doesn't exist create it
  if not os.path.exists(test_out_images_dir):
    os.makedirs(test_out_images_dir)

  status = save_image(canvas_win, 512, 512, image_filepath)
  #print "image saved:", status
  #Only compare if there was an _out file
    
  assert(png_imagediff(image_filepath , image_ref_filepath + "_ref") < 10)


def compare_output(canvas_win, dir, test, replace_ref):
  ref_filepath = os.path.join(canvas_ref_dir, dir, test)
  ref_file = ref_filepath + "_ref" + os.extsep + "out"
  # write the content of the test file to file if replace_ref and an .out exists
  if (not os.path.exists(ref_file)) or replace_ref:
    if canvas_win.test_output:  
      test_file = open(ref_file, "w")    
      test_file.write(canvas_win.test_output)
      test_file.close()
  
  if os.path.exists(ref_file):
    expected_file = open(ref_file, 'r')
    expected_contents = expected_file.read()
    expected_file.close()

    if canvas_win.test_output:
      assert (expected_contents == canvas_win.test_output)
    
        
def comparison(canvas_win, dir, test, replace_ref = False):
  canvas_win.settings.setValue("mainWindow/geometry", canvas_win.saveGeometry())
  canvas_win.settings.setValue("mainWindow/state", canvas_win.saveState())
  
  canvas_win.menuBar().setVisible(False)
  canvas_win.undoDockWidget.hide()
  canvas_win.logDockWidget.hide()
  canvas_win.renderingOptionsDockWidget.hide()
  canvas_win.valueEditorDockWidget.hide()
  canvas_win.dfgDock.hide()
  canvas_win.treeDock.hide()
  canvas_win.timeLineDock.hide()
  canvas_win.scriptEditorDock.hide()
  
  canvas_filepath = os.path.join(canvas_tests_dir, dir, test)
  canvas_win.onNewGraph(skip_save=True)  
  canvas_win.setTest(canvas_filepath) 
  
  canvas_win.scriptEditor.loadScript(canvas_filepath + os.extsep + "py")
  canvas_win.scriptEditor.execute()
  
  compare_images(canvas_win, dir, test, replace_ref)
  compare_output(canvas_win, dir, test, replace_ref)

  canvas_win.restoreGeometry(canvas_win.settings.value("mainWindow/geometry"))
  canvas_win.restoreState(canvas_win.settings.value("mainWindow/state"))  

# Defines programmatically all the test functions based on all the .canvas files given in a based dir (recursive)
def enumerate_canvas_tests(base_dir):
  canvas_tests = []
  for dir_name, _subdir_list, file_list in os.walk(base_dir):
    for file_name in file_list:
      file_base, file_ext = os.path.splitext(file_name)
      if file_ext == '.py':      
        canvas_tests += [(os.path.relpath(dir_name, base_dir), file_base)]
  return canvas_tests

canvas_tests = enumerate_canvas_tests(canvas_tests_dir)


for test in canvas_tests:  
  canvas_test = 'def test_' + test[1] + '(canvas_win, replace_ref = False): comparison(canvas_win, "' + test[0] + '", "' + test[1] + '", replace_ref)'
  exec(canvas_test)

  
# Manual execution
if __name__ == '__main__':  
  canvas_win_instance = canvas_win()
  parser = OptionParser()
  parser.add_option("-t", "--test", dest="test_name", help="The name of the canvas file to test. No Path required, and works with or without extension", default="")
  parser.add_option("-r", "--replace", dest="replace", help="Replace the given test outputs", default=False)
  options, args = parser.parse_args()
  if options.test_name:
    try:      
      update_ref = options.replace
      exec("test_" + os.path.splitext(options.test_name)[0] + "(canvas_win_instance, " + str(update_ref) + ")")
    except Exception as e:    
      print repr(e)
    #app.exec_()

  
