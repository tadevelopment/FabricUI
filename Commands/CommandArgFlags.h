//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_FLAGS__
#define __UI_COMMAND_FLAGS__

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

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_FLAGS__
