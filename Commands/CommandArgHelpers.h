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
      QMap<QString, QString> const&argsDesc,
      BaseCommand *cmd
      );

    /// Helper to create the command's helps
    static QString getArgsTypeSpecs(
      BaseCommand *cmd,
      QString const&key
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString createHelpFromArgs(
      QString const&commandHelp,
      QMap<QString, QString> const&argsHelp,
      BaseCommand *cmd
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString createHelpFromRTValArgs(
      QString const&commandHelp,
      QMap<QString, QString> const&argsHelp,
      BaseCommand *cmd
      );
 
    static bool isRTValScriptableCommand(
      BaseCommand *cmd
      );

    static bool isScriptableCommand(
      BaseCommand *cmd
      );
 
    static bool isPathValueCommandArg(
      QString const&key,
      BaseCommand *cmd
      );

    static bool isCommandArg(
      QString const&key,
      int flags,
      BaseCommand *cmd
      );

    static bool isCommandArgSet(
      QString const&key,
      BaseCommand *cmd
      );

    static bool hasCommandArg(
      QString const&key,
      BaseCommand *cmd
      );

    static QList<QString> getCommandArgKeys(
      BaseCommand *cmd
      );

    static QString getCommandArg(
      QString const&key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal getRTValCommandArg(
      QString const&key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal getRTValCommandArgValue(
      QString const&key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal getRTValCommandArgValue(
      QString const&key,
      QString const&type,
      BaseCommand *cmd
      );

    static QString getRTValCommandArgType(
      QString const&key,
      BaseCommand *cmd
      );

    // static QMap<QString, FabricCore::RTVal> castRTValArgs(
    //   BaseCommand *cmd,
    //   QMap<QString, QString> const&args
    //   );
};

} // namespace Commands
} // namespace FabricUI

#endif // _UI_COMMAND_ARG_HELPERS__
