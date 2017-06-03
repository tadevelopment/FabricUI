//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_ARG_HELPERS__
#define __UI_COMMAND_ARG_HELPERS__

#include <QMap>
#include <QString>

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

class BaseScriptableCommand;
class BaseRTValScriptableCommand;

class CommandArgHelpers 
{  
  public:
    /// Helper to create the command's desc
    /// from a subsets of arguments.
    /// \param argsHelp Map of [arg, arg desc]
    static QString CreateHistoryDescFromArgs(
      const QMap<QString, QString> &argsDesc,
      BaseScriptableCommand *cmd
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString CreateHelpFromArgs(
      const QString &commandHelp,
      const QMap<QString, QString> &argsHelp,
      BaseScriptableCommand *cmd
      );

    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    static QString CreateHelpFromRTValArgs(
      const QString &commandHelp,
      const QMap<QString, QString> &argsHelp,
      BaseRTValScriptableCommand *cmd
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_ARG_HELPERS__
