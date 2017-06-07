//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_SCRIPTABLE_COMMAND__
#define __UI_KL_SCRIPTABLE_COMMAND__

#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class KLScriptableCommand : public BaseRTValScriptableCommand
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
      KLCommandRegistry *registry = qobject_cast<KLCommandRegistry *>(
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
  
  Q_OBJECT
  
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
    virtual bool canLog();
    
    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

    /// Implementation of BaseCommand.
    virtual bool redoIt();
 
    /// Implementation of BaseCommand.
    virtual QString getHelp();

    /// Implementation of BaseCommand.
    virtual QString getHistoryDesc();

    /// Implementation of BaseCommand.
    virtual void setCanMergeID(
      int canMergeID
      );

    /// Implementation of BaseCommand.
    virtual int getCanMergeID();

    /// Implementation of BaseCommand.
    virtual bool canMerge(
      BaseCommand *prevCmd
      );

    /// Implementation of BaseCommand.
    virtual void merge(
      BaseCommand *prevCmd
      );

    /// Implementation of BaseScriptableCommand.
    /// Does nothing, the arg is set in KL.
    virtual void declareArg( 
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Implementation of BaseScriptableCommand.
    virtual QList<QString> getArgKeys();

    /// Implementation of BaseScriptableCommand.
    virtual bool isArgSet(
      const QString &key
      );

    /// Implementation of BaseScriptableCommand.
    virtual bool hasArg(
      const QString &key 
      );
    
    /// Implementation of BaseScriptableCommand.
    virtual bool isArg(
      const QString &key,
      int flag
      );

    /// Implementation of BaseScriptableCommand.
    /// Gets the RTVal JSON representation.
    virtual QString getArg( 
      const QString &key
      );

    /// Implementation of BaseScriptableCommand.
    /// Sets the RTVal from JSON.
    virtual void setArg(
      const QString &key, 
      const QString &json
      );

    /// Implementation of BaseScriptableCommand.
    virtual void validateSetArgs();

    /// Implementation of BaseScriptableCommand.
    virtual QString getArgsDescription();

    /// Implementation of BaseRTValScriptableCommand.
    /// Does nothing, the arg is set in KL.
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      int flags = FabricUI::Commands::CommandArgFlags::LOGGABLE_ARG, 
      FabricCore::RTVal defaultValue = FabricCore::RTVal()
      );

    /// Implementation of BaseRTValScriptableCommand.
    virtual QString getRTValArgType(
      const QString &key
      );
    
    /// Implementation of BaseRTValScriptableCommand.
    virtual QString getRTValArgPath( 
      const QString &key 
      );

    /// Implementation of BaseRTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArgValue( 
      const QString &key 
      );

    /// Implementation of BaseRTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArgValue( 
      const QString &key,
      const QString &type
      );

    /// Implementation of BaseRTValScriptableCommand.
    virtual void setRTValArgValue(
      const QString &key, 
      FabricCore::RTVal value
      );

    /// Implementation of BaseRTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArg(
      const QString &key 
      );

    /// Implementation of BaseRTValScriptableCommand.
    virtual void setRTValArg(
      const QString &key, 
      FabricCore::RTVal pathValue
      );

  private:
    /// KL BaseCommand
    FabricCore::RTVal m_klCmd;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_SCRIPTABLE_COMMAND__
