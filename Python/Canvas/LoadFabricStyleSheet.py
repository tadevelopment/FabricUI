import os
import platform


def LoadFabricStyleSheet(basename):
    """Loads the contents of the stylesheet and formats it for compatibility 
    with Fabric Engine widgets.
    
    Args:
        basename (str): Stylesheet base name.
    
    Returns:
        str: Formatted contents of the stylesheet file.
    
    """

    fabricDir = os.environ.get('FABRIC_DIR')
    if fabricDir is None:
        raise IOError("LoadFabricStyleSheet: missing FABRIC_DIR environment variable\n")
    
    qssPath = os.path.join(fabricDir, 
                           "Resources",
                           "QSS",
                           basename)

    qssData = ''

    try:
        with open(qssPath, "r") as qssFile:
            qssData = os.path.expandvars(qssFile.read())
    except EnvironmentError:
        print "LoadFabricStyleSheet: unable to open {}\n".format(qssPath)
        return qssData

    if qssData == '':
        print "LoadFabricStyleSheet: style sheet {} is empty\n".format(qssPath)
        return qssData

    if platform.system() == 'Windows':
        qssData = qssData.replace("\\", "/")

    return qssData