// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_DFG_COMMAND__
#define __UI_BASE_DFG_COMMAND__

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/Commands/BaseRTValScriptableCommand.h>

namespace FabricUI {
namespace DFG {

class BaseDFGCommand : public Commands::BaseRTValScriptableCommand 
{
  /**
    BaseDFGCommand is the base class for any DFGExec-based command.
    It has a pointer to the DFGController from where it can access
    the DFGExec and the DFGBinding. It manages the command undo-redo
    so speciaized classes only implement the command logic. Finally, 
    it uses JSON/RTVal-based arguments.

    Usage:
    - C++:
      class DFGCommand : public BaseDFGCommand 
      {
        public:
          DFGCommand() : BaseDFGCommand()
          {
            FabricCore::Client client = 
              Commands::CommandManager::GetCommandManager()->getClient();

            // String argument
            declareArg(
              "myArg", 
              "String", 
              false, 
              FabricCore::RTVal::ConstructString(
                client,
                "MyString")
            );
          }

          virtual bool doIt()
          {
            // Get the arguments 
            QString myArg = getRTValArg(
              "myArg"
              ).getStringCString();

            // Do some stuff with DFGExec
            m_dfgController->getExec().doSomeStuff( 
              myArg.toUtf8().constData(), 
              true);
            
            // Increment the counter.
            // Used to undo/redo the command.
            ++m_coreCmdCount;

            return true;
          } 
      };

      // Register the command
      DFGController *dfgController = ...

      Commands::CommandFactory<DFGCommand>::RegisterCommand(
        "dfgCommand",
        (void *)dfgController);
      
      // Create an execute the command
      FabricCore::Client client = CommandManager::GetCommandManager()->getClient(); 

      FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
        client, 
        "someString");
      
      QMap<QString, FabricCore::RTVal> args;
      args["myArg"] = strRTVal;
   
      CommandManager *manager = CommandManager.GetCommandManager();
      Command *cmd = manager->createRTValCommand("dfgCommand",  args);

    - Python:
      class DFGCommand(DFG.BaseDFGCommand): 
      
        def __init__(self):
          super(DFGCommand, self).__init__()
          client = GetCommandManager().getClient()

          // String argument
          self.declareArg(
            "myArg", 
            "String", 
            False, 
            client.RT.types.String("MyString")
          )
            
        def doIt(self):
          # Get the arguments 
          myArg = getRTValArg(
            "myArg"
            ).getSimpleType()

          # Do some stuff with DFGExec
          self.m_dfgController.getExec().doSomeStuff(myArg, true)
  
          self.m_coreCmdCount = self.m_coreCmdCount + 1

          return True
  
      // Register the command
      dfgController = ...

      GetCommandRegistry().registerCommand("dfgCommand", DFGCommand, dfgController)
      
      // Create an execute the command
      client = GetCommandManager().getClient()

      strRTVal = client.RT.types.String("someString")
      args = { "arg_1" : strRTVal }

      cmd = GetCommandManager().createCmd("dfgCommand", args) 
  */
  
  public:
    BaseDFGCommand();

    virtual ~BaseDFGCommand();
    
    /// Implementation of Command.
    /// `userData` is used to set the 
    /// pointer to the DFGController.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      );

    /// Implementation of Command.
    virtual bool canUndo();

    /// Implementation of Command.
    virtual bool undoIt();

    /// Implementation of Command.
    virtual bool redoIt();

  protected:
    unsigned m_coreCmdCount;
    DFGController *m_dfgController;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_BASE_DFG_COMMAND__
