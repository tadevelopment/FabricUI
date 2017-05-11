//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_FLAGS__
#define __UI_COMMAND_FLAGS__

namespace FabricUI {
namespace Commands {

class CommandFlags
{
  /**
    CommandFlags  
 
  */
  public:
  	CommandFlags() {}
    /// Optional argument flags
    static int OPTIONAL_ARG;
    /// Loggale argument flags
    static int LOGGABLE_ARG;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_FLAGS__
