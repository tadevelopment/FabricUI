# Returns the output of the test
def main() :
  from PySide import QtCore
  from FabricEngine import Core
  from FabricEngine.Canvas.CanvasWindow import CanvasWindow
  from FabricEngine.FabricUI import Application

  class CanvasTestWindow( CanvasWindow ) :
    storedOutput = ""
    def _reportCallback(self, source, level, line):
      prefix = "[FABRIC:MT] "
      if len(line) >= len(prefix) and line[:len(prefix)] == prefix :
        line = line[len(prefix):]
      self.storedOutput += line + '\n'

  app = Application.FabricApplication()
  canvas = CanvasTestWindow( QtCore.QSettings(), False, False )
  binding = canvas.dfgWidget.getDFGController().getBinding()
  ex = binding.getExec()

  names = [
    "timeline",
    "timelineStart",
    "timelineEnd",
    "timelineFramerate"
  ]
  for name in names :
    ex.addExecPort( name, canvas.client.DFG.PortTypes.In, "Float32" )

  funcName = "reportPorts"
  ex.addInstWithNewFunc( funcName )
  func = ex.getSubExec( funcName )

  for name in names :
    func.addExecPort( name, canvas.client.DFG.PortTypes.In, "Float32" )
    ex.connectTo( name, funcName + '.' + name )

  mainCode = "\n".join( [ " report( '" + name + " = ' + " + name + " );" for name in names ] )
  func.setCode("""
  dfgEntry
  {
  """ + mainCode + """
  report("");
  }
  """)

  ex.connectTo( funcName + '.exec', 'exec' )

  #print("TEST_START");
  canvas.storedOutput = ""


  # Default values
  QtCore.QCoreApplication.processEvents()

  # Time Range
  canvas.timeLine.setTimeRange( 11, 314 )
  QtCore.QCoreApplication.processEvents()

  # FrameRate
  canvas.timeLine.setFrameRate( 25 )
  QtCore.QCoreApplication.processEvents()

  # Back to an empty graph
  canvas.show()
  canvas.onNewGraph(True)

  #print("TEST_END");
  return canvas.storedOutput

def test_timeline_ports():

  import os

  refOutputPath = os.environ['FABRIC_SCENE_GRAPH_DIR'] + '/Native/FabricUI/Test/test_timeline_ports.out'
  refOutput = open( refOutputPath, 'r' ).read()

  refBaseName, refOutputExt = os.path.splitext( refOutputPath )
  if( os.path.exists( refBaseName + os.extsep + os.environ['FABRIC_BUILD_OS'] + os.extsep + "skip" ) ) :
    import pytest
    pytest.skip("Skip")

  # TODO: some of the output from the DFGController is not caught
  testOutput = main()

  if testOutput != refOutput :
    import difflib
    diff = difflib.unified_diff( refOutput.splitlines(1), testOutput.splitlines(1) )
    raise Exception( ''.join( diff ) )
