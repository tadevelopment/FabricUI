//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_FABRIC_APPLICATION_STATES__
#define __UI_FABRIC_APPLICATION_STATES__

#include <QSettings>
#include <FabricCore.h>
 
namespace FabricUI {
namespace Application {

class FabricApplicationStates  
{
  /**
    FabricApplicationStates is a default implementation of Command.
    New commands can inheritated this class.

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */

  public:
    FabricApplicationStates(
      FabricCore::Client client,
      QSettings *settings
      );

    virtual ~FabricApplicationStates();

    static FabricApplicationStates* GetAppStates();
 
    virtual FabricCore::Context getContext();
        
    virtual FabricCore::Client getClient();

    QSettings* getSettings();

  private:
    /// Name of the command.
    FabricCore::Client m_client;
    ///
    QSettings *m_settings;

    /// CommandManager singleton, set from Constructor.
    static FabricApplicationStates *s_appStates;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};

} // namespace Application
} // namespace FabricUI

#endif // __UI_FABRIC_APPLICATION_STATES__
