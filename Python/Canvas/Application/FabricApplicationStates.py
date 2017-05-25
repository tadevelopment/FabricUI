#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Application as CppApplication

class FabricApplicationStates(CppApplication.FabricApplicationStates):

    """ FabricApplicationStates registers commands used in the Canvas application. It specializes the C++ 
    """

    def __init__(self, client, settings): 
        """ Initializes the FabricApplicationStates.
        """
        super(FabricApplicationStates, self).__init__(client, settings)
     
        self.__client = client

    def getClient(self):
        return self.__client;
   
# \internal, store the registry singleton 
global s_appStatesSingleton
s_appStatesSingleton = None

def GetAppStates():
    """ Gets the FabricApplicationStates singleton.
        Raises an exception if the registry is not created. 
    """
    if s_appStatesSingleton is None:
        raise Exception('FabricApplicationStates.GetAppStates, the registry is null.\n\
            To create it : CreateAppStates(FabricCore.Client).')
    else:
        return s_appStatesSingleton

def CreateAppStates(client, settings):
    """ Creates the FabricApplicationStates singleton.
    """
    global s_appStatesSingleton
    if s_appStatesSingleton is None:
        s_appStatesSingleton = FabricApplicationStates(client, settings)
    return s_appStatesSingleton
