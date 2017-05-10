//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_HELPERS__
#define __UI_KL_COMMAND_HELPERS__

#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {

/**
	Helpers for KLCommand and KLScriptableCommands.
*/

/// Gets the name of a KL commands.
/// Throws an exception if an error occurs.
QString GetKLCommandName(
  FabricCore::RTVal klCmd
  );

/// Checks if the KL command is undoable.
/// Throws an exception if an error occurs.
bool CanKLCommandUndo(
  FabricCore::RTVal klCmd
  );

/// Executes a KL commands.
/// Throws an exception if an error occurs.
bool DoKLCommand(
  FabricCore::RTVal klCmd
  );

/// Undoes the current KL command.
/// Throws an exception if an error occurs.
bool UndoKLCommand();

/// Redoes the current KL command.
/// Throws an exception if an error occurs.
bool RedoKLCommand();

/// Gets a KL command's description.
/// Throws an exception if an error occurs.
QString GetKLCommandHelp(
  FabricCore::RTVal klCmd
  );

/// Gets a KL command's history description.
/// Throws an exception if an error occurs.
QString GetKLCommandHistoryDesc(
  FabricCore::RTVal klCmd
  );

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_HELPERS__
