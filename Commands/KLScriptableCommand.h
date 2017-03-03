//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_SCRIPTABLE_COMMAND__
#define __UI_KL_SCRIPTABLE_COMMAND__

#include <QList>
#include <FabricCore.h>
#include "BaseScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class KLScriptableCommand : public BaseScriptableCommand
{
  /**
    KLScriptableCommand wraps the KL scriptable commands so they  
    are accessible from C++/Python. They are created on-demand 
    by the  C++/Python command registry.

    In KL, the commands arguments are RTVal, and JSON format is 
    used to cast them into/from QString.

    Usage:
    - KL:
      object MyCommand : BaseScriptableCommand {
      };

      MyCommand() {
        this.declareArg(
          "arg_1",
          String,
          false,
          RTVal("arg_1_default"));

        this.declareArg(
          "arg_2",
          Float32,
          false,
          RTVal(Float32(1.0)));
      }

      // Register the command
      GetCommandRegistry().registerCommand(
        "myCommand", 
        MyCommand);

    - C++:
      FabricCore::Client client = CommandManager::GetCommandManager()->getFabricClient(); 

      // Create the arguments values as RTVal
      FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
        client, 
        "someString");

      FabricCore::RTVal floatRTVal = FabricCore::RTVal::ConstructFloat32(
        client, 
        4.555);
      
      // Cast them in JSON
      QMap<QString, QString> args;
      args["arg_1"] = strRTVal.getJSON().getStringCString();
      args["arg_2"] = floatRTVal.getJSON().getStringCString();
      
      // Create an execute the command
      CommandManager *manager = CommandManager.GetCommandManager();
      BaseCommand *cmd = manager->createCommand("myCommand", args);
      
      // Get back the arguments
      strRTVal.setJSON(cmd->getArg("arg_1"));
      floatRTVal.setJSON(cmd->getArg("arg_2"));

    - Python:
      client = GetCommandManager().getFabricClient()

      // Create the arguments values as RTVal
      strRTVal = client.RT.types.String("someString")
      floatRTVal = client.RT.types.Float32(4.555)

      // Cast them in JSON
      args = 
      { 
        "arg_1" : strRTVal.getJSONStr(),
        "arg_2" : floatRTVal.getJSONStr()
      }

      // Create an execute the command
      manager = GetCommandManager()
      cmd = manager.createCommand("myCommand", args) 

      // Get back the arguments
      strRTVal.setJSON(cmd.getArg("arg_1"))
      floatRTVal.setJSON(cmd.getArg("arg_2"))
  */

  public:
    KLScriptableCommand(
      FabricCore::RTVal klCmd
      );
    
    virtual ~KLScriptableCommand();

    /// Implementation of BaseCommand.
    virtual QString getName();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

    /// Implementation of BaseCommand.
    virtual bool redoIt();

    /// Implementation of BaseScriptableCommand.
    /// Does nothing, done in KL.
    virtual void declareArg( 
      const QString &key, 
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Implementation of BaseScriptableCommand.
    virtual QMap<QString, QString> getArgs();

    /// Implementation of BaseScriptableCommand.
    virtual QString getArg( 
      const QString &key 
      );

    /// Implementation of BaseScriptableCommand.
    virtual void setArg(
      const QString &key, 
      const QString &jsonRTVal
      );

    /// Implementation of BaseScriptableCommand.
    virtual void validateSetArgs();

    /// Implementation of BaseScriptableCommand.
    virtual QString getArgsDescription();
 
    /// Implementation of BaseScriptableCommand.
    virtual QString getHelp();

  private:
    /// KL Command
    FabricCore::RTVal m_klCmd;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_SCRIPTABLE_COMMAND__
