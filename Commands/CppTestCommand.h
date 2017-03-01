//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CPP_TEST_COMMAND_REGISTRY__
#define __UI_CPP_TEST_COMMAND_REGISTRY__

#include <QString>
#include <FabricCore.h>
#include "CommandFactory.h"
#include "CommandManager.h"
#include "CommandRegistry.h"
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
      CommandFactory<CppTestScriptableCommand>::RegisterCommand("cppTestScriptableCommand");
      CommandFactory<CppTestMetaCommand>::RegisterCommand("cppTestMetaCommand");
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
        FabricCore::RTVal CppToKLTestCommand = FabricCore::RTVal::Create(
          CommandRegistry::GetCommandRegistry()->getFabricClient(), 
          "CppToKLTestCommand", 
          0, 
          0);

        CppToKLTestCommand.callMethod(
          "", 
          "registerKLTestCommands", 
          0, 
          0);  
      }    

      catch(FabricCore::Exception &e)
      {
        printf(
          "RegisterKLTestCommands: exception: %s\n", 
          e.getDesc_cstr());
      }

      // Synchronise the stack
      CommandRegistry::GetCommandRegistry()->synchronizeKL();
    }

    /// Creates KL commands from KL.
    static void CreateKLTestCommandsInKL() 
    {
      try
      {
        FabricCore::RTVal CppToKLTestCommand = FabricCore::RTVal::Create(
          CommandRegistry::GetCommandRegistry()->getFabricClient(), 
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
        CommandManager::GetCommandManager()->createCommand("klTestCommand");
        CommandManager::GetCommandManager()->createCommand("klTestMetaCommand");

        FabricCore::RTVal strRTVal = FabricCore::RTVal::ConstructString(
          CommandRegistry::GetCommandRegistry()->getFabricClient(), 
          "string_Cpp");

        FabricCore::RTVal floatRTVal = FabricCore::RTVal::ConstructFloat32(
          CommandRegistry::GetCommandRegistry()->getFabricClient(), 
          4.555f);
     
        QMap<QString, QString> args;
        args["arg_1"] = strRTVal.getJSON().getStringCString();
        args["arg_2"] = floatRTVal.getJSON().getStringCString();

        CommandManager::GetCommandManager()->createCommand(
          "klTestScriptableCommand", 
          args);

        CommandManager::GetCommandManager()->createCommand(
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
