//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_SCRIPTABLE_COMMAND__
#define __UI_KL_SCRIPTABLE_COMMAND__

#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class KLScriptableCommand
  : public BaseRTValScriptableCommand
{
  /**
    KLScriptableCommand wraps the KL scriptable commands so they  
    are accessible from C++/Python. They are created on-demand 
    by the  C++/Python command registry.

    In KL, the commands arguments are RTVal, and JSON format is 
    used to cast them into/from QString.

    Usage:
    - KL:
      object MyCommand : BaseRTValScriptableCommand {
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
      KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
        Commands::CommandRegistry::GetCommandRegistry());

      // Create the arguments values as RTVal
      FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
        Application::FabricApplicationStates::GetAppStates()->getContext(), 
        "someString");

      FabricCore::RTVal floatRTVal = FabricCore::RTVal::ConstructFloat32(
        Application::FabricApplicationStates::GetAppStates()->getContext(), 
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

    /// Implementation of Command.
    virtual QString getName();

    /// Implementation of Command.
    virtual bool canUndo();

    /// Checks if the KL command can be 
    /// added to the KL undo stack.
    bool addToUndoStack();

    /// Implementation of Command.
    virtual bool canLog();
    
    /// Implementation of Command.
    virtual bool doIt();

    /// Implementation of Command.
    virtual bool undoIt();

    /// Implementation of Command.
    virtual bool redoIt();
 
    /// Implementation of Command.
    virtual QString getHelp();

    /// Implementation of Command.
    virtual QString getHistoryDesc();

    /// Implementation of ScriptableCommand.
    /// Does nothing, the arg is set in KL.
    virtual void declareArg( 
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Implementation of ScriptableCommand.
    virtual QList<QString> getArgKeys();

    /// Implementation of ScriptableCommand.
    virtual bool isArgSet(
      const QString &key
      );

    /// Implementation of ScriptableCommand.
    virtual bool hasArg(
      const QString &key 
      );
    
    /// Implementation of ScriptableCommand.
    virtual bool isArg(
      const QString &key,
      int flag
      );

    /// Implementation of ScriptableCommand.
    /// Gets the RTVal JSON representation.
    virtual QString getArg( 
      const QString &key
      );

    /// Implementation of ScriptableCommand.
    /// Sets the RTVal from JSON.
    virtual void setArg(
      const QString &key, 
      const QString &json
      );

    /// Implementation of ScriptableCommand.
    virtual void validateSetArgs();

    /// Implementation of ScriptableCommand.
    virtual QString getArgsDescription();

    /// Implementation of RTValScriptableCommand.
    /// Does nothing, the arg is set in KL.
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      int flag, 
      FabricCore::RTVal defaultValue
      );

    /// Implementation of RTValScriptableCommand.
    virtual QString getRTValArgType(
      const QString &key
      );

    /// Implementation of RTValScriptableCommand.
    /// Does nothing, the arg is set in KL.
    virtual void setRTValArgType(
      const QString &key,
      const QString &type
      );
    
    /// Implementation of RTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key 
      );

    /// Implementation of RTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key,
      const QString &type
      );

    /// Implementation of RTValScriptableCommand.
    /// Does nothing, the arg is set in KL.
    virtual void setRTValArg(
      const QString &key, 
      FabricCore::RTVal value
      );

  private:
    /// KL Command
    FabricCore::RTVal m_klCmd;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_SCRIPTABLE_COMMAND__
