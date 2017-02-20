#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import sys, imp

def GetOrCreateModule(name):
    """ Get or creates dynamically a module named "name".
    	- module = GetOrCreateModule(moduleName)

    	Once the module has been created, it can be imported the usual way:
    	- import module
    """
    
    isDefined = True
    try:
        module = sys.modules[name]
    except Exception, e:
        isDefined = False

    if isDefined is False:
        module = imp.new_module(name)
        sys.modules[name] = module

    return sys.modules[name]
