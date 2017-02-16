#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import sys
 
class GlobalInjector:
    ''' Inject into the *real global namespace*, i.e. "builtins" namespace or "__builtin__" for python2.
        Assigning to variables declared global in a function, injects them only into the module's global namespace.
        - Global= sys.modules['__builtin__'].__dict__
        - #would need 
        - Global['aname'] = 'avalue'
        - #With
        - Global = GlobalInjector()
        - #one can do
        - Global.bname = 'bvalue'
        - #reading from it is simply
        - bname
        bvalue
    '''
    def __init__(self):
        try:
            self.__dict__['builtin'] = sys.modules['__builtin__'].__dict__
        except KeyError:
            self.__dict__['builtin'] = sys.modules['builtins'].__dict__
    
    def __setattr__(self,name,value):
        self.builtin[name] = value
