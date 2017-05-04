//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_HELPERS__
#define __UI_KL_COMMAND_HELPERS__

#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {
 
QString GetKLCommandName(
  FabricCore::RTVal klCmd
  );

bool CanKLCommandUndo(
  FabricCore::RTVal klCmd
  );

bool DoKLCommand(
  FabricCore::RTVal klCmd
  );

bool UndoKLCommand();

bool RedoKLCommand();

QString GetKLCommandHelp(
  FabricCore::RTVal klCmd
  );

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_HELPERS__
