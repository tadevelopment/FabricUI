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

/// Gets the KL command manager singleton.
FabricCore::RTVal GetKLCommandManager();

/// Gets the KL command registry singleton.
FabricCore::RTVal GetKLCommandRegistry();

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

/// Checks if the KL command can be 
/// added to the KL undo stack.
/// Throws an exception if an error occurs.
bool AddKLCommandToUndoStack(
  FabricCore::RTVal klCmd
  );

/// Checks if the KL command is undoable.
/// Throws an exception if an error occurs.
bool CanKLCommandLog(
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

/// Sets the interaction ID of a KL commands.
/// Throws an exception if an error occurs.
void SetKLCommandCanMergeID(
  FabricCore::RTVal klCmd,
  int canMergeID
  );

/// Gets the interaction ID of a KL commands.
/// Throws an exception if an error occurs.
int GetKLCommandCanMergeID(
  FabricCore::RTVal klCmd
  );

/// Merges a KL command with another.
/// Throws an exception if an error occurs.
bool CanMergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal otherKLCmd
  );

/// Merges a KL command with another.
/// Throws an exception if an error occurs.
void MergeKLCommand(
  FabricCore::RTVal klCmd,
  FabricCore::RTVal otherKLCmd
  );

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_HELPERS__
