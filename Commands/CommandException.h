//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_HELPERS__
#define __UI_COMMAND_HELPERS__

#include <stdio.h>
#include <QString>
#include <exception>

namespace FabricUI {
namespace Commands {

#define NOTHING 0
#define PRINT 1
#define THROW 2

class CommandException : public std::exception
{
  /**
    Exception for commands.
  */

  public: 
    CommandException(
      const QString &message)
      : m_message(message)
    {
    }
 
    virtual ~CommandException() throw() {}

    /// Trows and/or prints a CommandException.
    /// \param method Name of the method that fails.
    /// \param error The error to throw/print.
    /// \param childError A child error
    /// \param flag (THROW, PRINT)
    static QString Throw(
      const QString &method,
      const QString &error = QString(),
      const QString &childError = QString(),
      int flag = THROW)
    {
      QString cmdError = method + ", error: " + error + "\n ";
      if(!childError.isEmpty()) 
        cmdError += childError + "\n ";

      if(flag & PRINT)
        printf("%s", cmdError.toUtf8().constData());

      if(flag & THROW)
        throw CommandException(cmdError);

      return cmdError;
    }

    /// Implementation of exception.
    virtual const char* what() const throw()
    {
      return m_message.toUtf8().constData();
    }

  private:
    QString m_message;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_HELPERS__
