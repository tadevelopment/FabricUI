from FabricEngine import Core
import FabricEngine.FabricUI
from PySide.QtGui import QColor, QFont

def main():

  Config = FabricEngine.FabricUI.Util.Config
  rootCfg = Config()
  cfg = rootCfg.getOrCreateSection( "TestSection" )
  cfg.setAccess( Config.Access.ReadWrite )

  def assertEqual( a, b ) :
    assert a == b

  # tests if the value is the same when exported and imported
  def testValues( method, value1, value2 ) :

    v = method( method.__name__ + ".test", value1 )
    assertEqual( value1, v )

    v = method( method.__name__ + ".test", value2 )
    assertEqual( value1, v )

    v = method( method.__name__ + ".otherTest", value2 )
    assertEqual( value2, v )

  testValues( cfg.getOrCreateBool, True, False )
  testValues( cfg.getOrCreateDouble, 3.14, -1.71 )
  #testValues( cfg.getOrCreateFloat, -7.953, 1 ) # doesn't work : there are no Float32 in Python
  testValues( cfg.getOrCreateInt, -185, 3347 )
  testValues( cfg.getOrCreateQColor, QColor( 12, 84, 63 ), QColor( 255, 128, 200 ) )
  testValues( cfg.getOrCreateQString,  "text1", "${ANOTHER}/Text" )
  testValues( cfg.getOrCreateQFont, QFont( "Roboto", 10 ), QFont() )

  del rootCfg

main()
# the second time, it will return the persisted
# values (from the config file) instead of the default values
main()
