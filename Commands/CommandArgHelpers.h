//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_ARG_HELPERS__
#define __UI_COMMAND_ARG_HELPERS__

#include <QStringList>
#include "BaseScriptableCommand.h"
#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

/// Helper to create the command's desc
/// from a subsets of arguments.
/// \param argsHelp Map of [arg, arg desc]
QString CreateHistoryDescFromArgs(
  const QMap<QString, QString> &argsDesc,
  BaseScriptableCommand *cmd
  );

/// Helper to create the command's helps
/// from a subsets of arguments.
/// \param commandHelp The main help text.
/// \param argsHelp Map of [arg, arg help]
QString CreateHelpFromArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp,
  BaseScriptableCommand *cmd
  );

/// Helper to create the command's helps
/// from a subsets of arguments.
/// \param commandHelp The main help text.
/// \param argsHelp Map of [arg, arg help]
QString CreateHelpFromRTValArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp,
  BaseRTValScriptableCommand *cmd
  );

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_ARG_HELPERS__
