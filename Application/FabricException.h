//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_FABRIC_EXCEPTION__
#define __UI_FABRIC_EXCEPTION__

#include <QString>
#include <iostream>
#include <exception>
#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace Application {

#define NOTHING 0
#define PRINT 1
#define THROW 2

class FabricException : public std::exception
{
  /**
    FabricException is the exception class for Fabric applications.
    It provides a static method `Throw` that propagates the exceptions
    in order to provide the complete code-path of the error.

    It defines a macro FABRIC_CATCH_BEGIN() - FABRIC_CATCH_END() that catchs: 
    - JSONException
    - FabricException
    - FabricCore::Exception
  */

  public: 
    FabricException(
      QString message)
      : m_message(message)
    {
    }
 
    virtual ~FabricException() throw() {}

    /// Throws and/or prints a FabricException.
    /// \param method Name of the method that fails.
    /// \param error The error to throw/print.
    /// \param childError A child error
    /// \param flag (THROW, PRINT)
    static QString Throw(
      QString method,
      QString error = QString(),
      QString childError = QString(),
      int flag = THROW)
    {
      QString cmdError = method + ", error: " + error + "\n ";
      if(!childError.isEmpty()) 
        cmdError += childError + "\n ";

      if(flag & PRINT)
        std::cerr << cmdError.toUtf8().constData() << std::endl;

      if(flag & THROW)
        throw FabricException(cmdError);

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

#define FABRIC_CATCH_BEGIN() \
  try {

#define FABRIC_CATCH_END(methodName) \
  } \
  catch (FabricCore::Exception &e) \
  { \
    FabricUI::Application::FabricException::Throw( \
      methodName, \
      "Caught Core Exception", \
      e.getDesc_cstr() \
      ); \
  } \
  catch (FTL::JSONException &je) \
  { \
    FabricUI::Application::FabricException::Throw( \
      methodName, \
      "Caught JSON Exception", \
      je.getDescCStr() \
      ); \
  } \
  catch (FabricUI::Application::FabricException &e) \
  { \
    FabricUI::Application::FabricException::Throw( \
      methodName, \
      "Caught App Exception", \
      e.what() \
      ); \
  } 

#endif // __UI_FABRIC_EXCEPTION__
