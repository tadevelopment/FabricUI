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
    /// Optional argument flag.
    static int OPTIONAL_ARG;
    /// Loggale argument flag.
    static int LOGGABLE_ARG;
    /// Input argument flag.
    static int IN_ARG;
    /// Output argument flag.
    static int OUT_ARG;
    /// IO argument flag.
    static int IO_ARG;
};

class BaseCommand;
 
class CommandArgHelpers 
{  
  public:
    /// Helper to create the command's desc
    /// from a subsets of arguments.
    /// \param argsHelp Map of [arg, arg desc]
    static QString CreateHistoryDescFromArgs(
      const QMap<QString, QString> &argsDesc,
      BaseCommand *cmd
      );

    /// Helper to create the command's helps
    static QString GetArgsTypeSpecs(
      BaseCommand *cmd,
      const QString &key
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString CreateHelpFromArgs(
      const QString &commandHelp,
      const QMap<QString, QString> &argsHelp,
      BaseCommand *cmd
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString CreateHelpFromRTValArgs(
      const QString &commandHelp,
      const QMap<QString, QString> &argsHelp,
      BaseCommand *cmd
      );
 
    static bool IsRTValScriptableCommand(
      BaseCommand *cmd
      );

    static bool IsScriptableCommand(
      BaseCommand *cmd
      );
 
    static bool IsPathValueCommandArg(
      const QString &key,
      BaseCommand *cmd
      );

    static bool IsCommandArg(
      const QString &key,
      int flags,
      BaseCommand *cmd
      );

    static bool IsCommandArgSet(
      const QString &key,
      BaseCommand *cmd
      );

    static bool HasCommandArg(
      const QString &key,
      BaseCommand *cmd
      );

    static QList<QString> GetCommandArgKeys(
      BaseCommand *cmd
      );

    static QString GetCommandArg(
      const QString &key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal GetRTValCommandArg(
      const QString &key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal GetRTValCommandArgValue(
      const QString &key,
      BaseCommand *cmd
      );

    static FabricCore::RTVal GetRTValCommandArgValue(
      const QString &key,
      const QString &type,
      BaseCommand *cmd
      );

    static QString GetRTValCommandArgType(
      const QString &key,
      BaseCommand *cmd
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // _UI_COMMAND_ARG_HELPERS__
