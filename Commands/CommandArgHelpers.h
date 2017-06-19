//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef _UI_COMMAND_ARG_HELPERS__
#define _UI_COMMAND_ARG_HELPERS__

#include <QMap>
#include <QObject>
#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {

class CommandArgFlags
{
  /**
    CommandArgFlags  
 
  */
  public:
    /// No flag argument.
    static int NO_FLAG_ARG;
    /// Optional argument.
    static int OPTIONAL_ARG;
    /// No-loggale argument.
    static int DONT_LOG_ARG;
    /// Input argument.
    static int IN_ARG;
    /// Output argument.
    static int OUT_ARG;
    /// IO argument.
    static int IO_ARG;
};

class BaseCommand;
 
class CommandArgHelpers 
{  
  public:
    /// Helper to create the command's desc
    /// from a subsets of arguments.
    /// \param argsHelp Map of [arg, arg desc]
    static QString createHistoryDescFromArgs(
      QMap<QString, QString> argsDesc,
      BaseCommand *cmd
      );

    /// Helper to create the command's helps
    static QString getArgsTypeSpecs(
      BaseCommand *cmd,
      QString key
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString createHelpFromArgs(
      QString commandHelp,
      QMap<QString, QString> argsHelp,
      BaseCommand *cmd
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString createHelpFromRTValArgs(
      QString commandHelp,
      QMap<QString, QString> argsHelp,
      BaseCommand *cmd
      );
 
    static bool isRTValScriptableCommand(
      BaseCommand *cmd
      );

    static bool isScriptableCommand(
      BaseCommand *cmd
      );
 
    static bool isPathValueCommandArg(
      QString key,
      BaseCommand *cmd
      );

    static bool isCommandArg(
      QString key,
      int flags,
      BaseCommand *cmd
      );

    static bool isCommandArgSet(
      QString key,
      BaseCommand *cmd
      );

    static bool hasCommandArg(
      QString key,
      BaseCommand *cmd
      );

    static QList<QString> getCommandArgKeys(
      BaseCommand *cmd
      );

    static QString getCommandArg(
      QString key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal getRTValCommandArg(
      QString key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal getRTValCommandArgValue(
      QString key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal getRTValCommandArgValue(
      QString key,
      QString type,
      BaseCommand *cmd
      );

    static QString getRTValCommandArgType(
      QString key,
      BaseCommand *cmd
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // _UI_COMMAND_ARG_HELPERS__
