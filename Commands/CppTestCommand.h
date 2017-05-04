//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CPP_TEST_COMMAND_REGISTRY__
#define __UI_CPP_TEST_COMMAND_REGISTRY__

#include <QString>
#include <FabricCore.h>
#include "KLCommandManager.h"
#include "KLCommandRegistry.h"
#include "BaseScriptableCommand.h"

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
        false,
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
      QMap<QString, QString> map;
      map["arg_1"] = "string_Cpp";

      CommandManager::GetCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        map);

      CommandManager::GetCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        map);

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
      QMap<QString, QString> map;
      map["arg_1"] = "string_Cpp";

      CommandManager::GetCommandManager()->createCommand(
        "cppTestScriptableCommand", 
        map);

      CommandManager::GetCommandManager()->createCommand(
        "cppTestMetaCommand");
    }

    /// Creates python commands from C++.
    static void CreatePythonTestCommands() 
    {
      QMap<QString, QString> map;
      map["arg_1"] = "string_Cpp";

      CommandManager::GetCommandManager()->createCommand(
        "testScriptableCommand", 
        map);

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
          Commands::CommandRegistry::GetCommandRegistry());

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
        printf(
          "RegisterKLTestCommands: exception: %s\n", 
          e.getDesc_cstr());
      }
    }

    /// Creates KL commands from KL.
    static void CreateKLTestCommandsInKL() 
    {
      try
      {
        KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
          Commands::CommandRegistry::GetCommandRegistry());

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
        printf(
          "CreateKLTestCommandsInKL: exception: %s\n", 
          e.getDesc_cstr());
      }
    }

    /// Creates KL commands from C++.
    static void CreateKLTestCommands() 
    {
      try
      {
        KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
          Commands::CommandManager::GetCommandManager());

        manager->createCommand("klTestCommand");
        manager->createCommand("klTestMetaCommand");
 
        FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
          manager->getClient(), 
          "string_Cpp");

        FabricCore::RTVal floatRTVal = FabricCore::RTVal::ConstructFloat32(
          manager->getClient(), 
          4.555f);
     
        QMap<QString, FabricCore::RTVal> args;
        args["arg_1"] = strRTVal;
        args["arg_2"] = floatRTVal;

        manager->createRTValCommand(
          "klTestScriptableCommand", 
          args);

        manager->createCommand(
          "klTestScriptableMetaCommand");
      }

      catch(FabricCore::Exception &e)
      {
        printf(
          "CreateKLTestCommands: exception: %s\n", 
          e.getDesc_cstr());
      }
    }
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_CPP_TEST_COMMAND_REGISTRY__
