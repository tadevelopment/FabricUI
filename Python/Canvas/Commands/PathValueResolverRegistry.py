#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands as CppCommands

class PathValueResolverRegistry(CppCommands.PathValueResolverRegistry):

    """ 
    """
    def __init__(self): 
        """ Initializes the PathValueResolverRegistry.
        """
        super(PathValueResolverRegistry, self).__init__()
        # There is no "new" in python, we need to own the resolver 
        # created in Python. They are referenced in the C++ registry. 
        self.__flatResolverStack = {}

    def registerResolver(self, name, resolverType, userData = None):
        """ Registers a Python PathValue resolver 'resolverType' under the name
            "name" and creates it. Raises an exception if the name is empty or 
            the resolverType is not a PathValueResolver.
        """
        if not name:
          raise Exception('PathValueResolverRegistry.registerResolver, error: name is empty') 

        if not issubclass(resolverType, CppCommands.BaseCommand):
            raise Exception(
                "PathValueResolverRegistry.registerResolver, error: '" + 
                str(name) + "': type '" + 
                str(resolverType) + "' is not a PathValueResolver")

        if super(PathValueResolverRegistry, self).hasResolver(name) is False:
            resolver = resolverType() 
            resolver.registrationCallback(name, userData)
            self.registerResolver(resolver, name)
            self.__flatResolverStack[name] = resolver

    def getResolver(self, name):
        if name in __flatResolverStack:
            return self.__flatResolverStack[name]
        else:
            return super(PathValueResolverRegistry, self).getResolver(name)
             
    def unregisterFactory(self, name):
        if name in __flatResolverStack:
            del __flatResolverStack[name]
        super(PathValueResolverRegistry, self).unregisterFactory(name)

# \internal, store the registry singleton 
global s_pathValueResolverRegistrySingleton
s_pathValueResolverRegistrySingleton = None

def GetPathValueResolverRegistry():
    """ Creates the PathValueResolverRegistry singleton.
    """
    global s_pathValueResolverRegistrySingleton
    if s_pathValueResolverRegistrySingleton is None:
        s_pathValueResolverRegistrySingleton = PathValueResolverRegistry()
    return s_pathValueResolverRegistrySingleton
