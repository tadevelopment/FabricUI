//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CPP_TEST_COMMAND_REGISTRY__
#define __UI_CPP_TEST_COMMAND_REGISTRY__

#include "BaseScriptableCommand.h"
#include "CommandManager.h"
#include "CommandRegistry.h"
#include "CommandFactory.h"
#include <QString>

namespace FabricUI {
namespace Commands {

class CppTestCommand : public BaseScriptableCommand 
{
	/**
		Use for unit-tests.
	*/

 	public:
    CppTestCommand() 
    	: BaseScriptableCommand()
    {
  	 	BaseScriptableCommand::declareArg(
       	"arg_1",
    		false,
       	"arg_1_default");
    }

    virtual ~CppTestCommand() 
    {
    }
 
    virtual bool canUndo() const
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

class CppTestMetaCommand : public CppTestCommand 
{
 	public:
    CppTestMetaCommand() 
    	: CppTestCommand()
    {
    }

    virtual ~CppTestMetaCommand() 
    {
    }
 
    virtual bool doIt()
    {
  		QMap<QString, QString> map;
			map["arg_1"] = "fooCpp";
    	CommandManager::GetCommandManager()->createCommand("cppTestCommand", map);
    	CommandManager::GetCommandManager()->createCommand("cppTestCommand", map);
			return true;
    }
};

class CppToPythonTestCommand
{
	public:
		CppToPythonTestCommand() 
		{
		}

		static void RegisterCppTestCommands() 
		{
			CommandFactory<CppTestCommand>::RegisterCommand("cppTestCommand");
			CommandFactory<CppTestMetaCommand>::RegisterCommand("cppTestMetaCommand");
		}

		static void CreatePythonTestCommands() 
		{
			QMap<QString, QString> map;
			map["arg_1"] = "fooCpp";
    	CommandManager::GetCommandManager()->createCommand("testScriptableCommand", map);
		}
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_CPP_TEST_COMMAND_REGISTRY__
