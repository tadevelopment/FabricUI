//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CPP_TEST_COMMAND_REGISTRY__
#define __UI_CPP_TEST_COMMAND_REGISTRY__

#include "KLCommandRegistry.h"
#include "RTValCommandManager.h"
#include "BaseScriptableCommand.h"
#include <FabricUI/Util/FabricException.h>

namespace FabricUI {
namespace Commands {

/**
  For unit-tests.
*/
class CppTestScriptableCommand : public BaseScriptableCommand 
{
  public:
    CppTestScriptableCommand() 
      : BaseScriptableCommand()
    {
      BaseScriptableCommand::declareArg(
        "arg_1",
        CommandArgFlags::OPTIONAL_ARG | CommandArgFlags::LOGGABLE_ARG,
        "arg_1_default_Cpp");
    }

    virtual ~CppTestScriptableCommand() 
    {
    }
 
    virtual bool canUndo()
    {
      return true;
    }
 
    virtual bool doIt()
    {
      return true;
    }
 
    virtual bool undoIt()
    {
      std::cout << "Undo " << getName().toUtf8().constData() << std::endl;
      return true;
    }
 
    virtual bool redoIt()
    {
      std::cout << "Redo " << getName().toUtf8().constData() << std::endl;
      return true;
    }

    virtual QString getHelp() 
    {
      return getName();
    }
};

class CppTestMetaCommand : public CppTestScriptableCommand 
{
  public:
    CppTestMetaCommand() 
      : CppTestScriptableCommand()
    {
    }

    virtual ~CppTestMetaCommand() 
    {
    }
 
    virtual bool doIt()
    {
      QMap<QString, QString> args;
      args["arg_1"] = "string_Cpp";

      CommandManager::GetCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        args);

      CommandManager::GetCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        args);

      return true;
    }
};

class CppToPythonTestCommand
{
  public:
    CppToPythonTestCommand()
    {
    }

    /// Register C++ commands to create them in python.
    static void RegisterCppTestCommands() 
    {
      CommandFactory<CppTestScriptableCommand>::Register("cppTestScriptableCommand");
      CommandFactory<CppTestMetaCommand>::Register("cppTestMetaCommand");
    }

    /// Creates C++ commands from C++.
    static void CreateCppTestCommands() 
    {
      QMap<QString, QString> args;
      args["arg_1"] = "string_Cpp";

      CommandManager::GetCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        args);

      CommandManager::GetCommandManager()->createCommand(
        "cppTestMetaCommand");
    }

    /// Creates python commands from C++.
    static void CreatePythonTestCommands() 
    {
      QMap<QString, QString> args;
      args["arg_1"] = "string_Cpp";

      CommandManager::GetCommandManager()->createCommand(
        "testScriptableCommand", 
        args);

      CommandManager::GetCommandManager()->createCommand(
        "testMetaCommand");
    }

    /// Register the KL commands in KL.
    /// see Ext/Builtin/FabbricInterfaces/Commands/KLTestCommands.kl.
    static void RegisterKLTestCommands() 
    {
      try
      {
        KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
          CommandRegistry::GetCommandRegistry());

        FabricCore::RTVal CppToKLTestCommand = FabricCore::RTVal::Create(
          registry->getClient(), 
          "CppToKLTestCommand", 
          0, 
          0);

        CppToKLTestCommand.callMethod(
          "", 
          "registerKLTestCommands", 
          0, 
          0);  

        // Synchronise the stack
        registry->synchronizeKL();
      }    

      catch(FabricCore::Exception &e)
      {
        Util::FabricException::Throw(
          "CppToPythonTestCommand::RegisterKLTestCommands",
          e.getDesc_cstr(),
          "",
          PRINT);
      }
    }

    /// Creates KL commands from KL.
    static void CreateKLTestCommandsInKL() 
    {
      try
      {
        KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
          CommandRegistry::GetCommandRegistry());

        FabricCore::RTVal CppToKLTestCommand = FabricCore::RTVal::Create(
          registry->getClient(), 
          "CppToKLTestCommand", 
          0, 
          0);

        CppToKLTestCommand.callMethod(
          "", 
          "createKLTestCommands", 
          0, 
          0);  
      }

      catch(FabricCore::Exception &e)
      {
        Util::FabricException::Throw(
          "CppToPythonTestCommand::CreateKLTestCommandsInKL",
          e.getDesc_cstr(),
          "",
          PRINT);
      }
    }

    /// Creates KL commands from C++.
    static void CreateKLTestCommands() 
    {
      try
      {
        RTValCommandManager *manager = (RTValCommandManager *)(
          CommandManager::GetCommandManager());

        manager->CommandManager::createCommand("klTestCommand");
        manager->CommandManager::createCommand("klTestMetaCommand");
 
        FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
          manager->getClient(), 
          "string_Cpp");

        FabricCore::RTVal floatRTVal = FabricCore::RTVal::ConstructFloat64(
          manager->getClient(), 
          4.555f);
     
        QMap<QString, FabricCore::RTVal> args;
        args["arg_1"] = strRTVal;
        args["arg_2"] = floatRTVal;

        manager->createCommand(
          "klTestScriptableCommand", 
          args);

        manager->CommandManager::createCommand(
          "klTestScriptableMetaCommand");
      }

      catch(FabricCore::Exception &e)
      {
        Util::FabricException::Throw(
          "CppToPythonTestCommand::CreateKLTestCommands",
          e.getDesc_cstr(),
          "",
          PRINT);
      }
    }
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_CPP_TEST_COMMAND_REGISTRY__
