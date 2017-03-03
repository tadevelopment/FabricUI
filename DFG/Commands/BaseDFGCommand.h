// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_DFG_COMMAND__
#define __UI_BASE_DFG_COMMAND__

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/Commands/BaseScriptableCommand.h>

namespace FabricUI {
namespace DFG {

class BaseDFGCommand : public Commands::BaseScriptableCommand 
{
  public:
    /**
      BaseDFGCommand is the base class for any DFGExec-based command.
      It has a pointer to the DFGController from where it can access
      the DFGExec and the DFGBinding. It manages the command undo-redo
      so speciaized class only implement the command logic.

      Because the core use

      Usage:
      - C++:
        class DFGCommand : public BaseDFGCommand 
        {
          public:
            DFGCommand() : BaseDFGCommand()
            {
              // String argument
              declareArg("myArg", false, "MyString");
            }

            virtual bool doIt()
            {
              // Get the arguments 
              QString myArg = Util::RTValUtil::jsonToRTVal(
                Commands::CommandManager::GetCommandManager()->getFabricClient(),
                getArg("myArg"),
                "String"
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
        FabricCore::Client client = CommandManager::GetCommandManager()->getFabricClient(); 

        FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
          client, 
          "someString");
        
        QMap<QString, QString> args;
        args["myArg"] = strRTVal.getJSON().getStringCString();
     
        CommandManager *manager = CommandManager.GetCommandManager();
        BaseCommand *cmd = manager->createCommand("dfgCommand",  args);

      - Python:
        class DFGCommand(DFG.BaseDFGCommand): 
        
          def __init__(self):
            super(DFGCommand, self).__init__()
            self.declareArg("myArg", False, ".")
       
          def doIt(self):
            # Get the arguments 
            myArg = Util.RTValUtil.jsonToRTVal(
              GetCommandManager().getFabricClient(),
              self.getArg("myArg"),
              "String"
              ).getSimpleType()

            # Do some stuff with DFGExec
            self.m_dfgController.getExec().doSomeStuff(myArg, true)
    
            self.m_coreCmdCount = self.m_coreCmdCount + 1

            return True
    
        // Register the command
        dfgController = ...

        GetCommandRegistry().registerCommand("dfgCommand", DFGCommand, dfgController)
        
        // Create an execute the command
        client = GetCommandManager().getFabricClient()

        strRTVal = client.RT.types.String("someString")
        args = { "arg_1" : strRTVal.getJSONStr() }

        cmd = GetCommandManager().createCommand("dfgCommand", args) 
    */

    BaseDFGCommand();

    virtual ~BaseDFGCommand();
    
    /// Implementation of BaseCommand.
    /// `userData` is used to set the 
    /// pointer to the DFGController.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      );

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

    /// Implementation of BaseCommand.
    virtual bool redoIt();

  protected:
    unsigned m_coreCmdCount;
    DFGController *m_dfgController;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_BASE_DFG_COMMAND__