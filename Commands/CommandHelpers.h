//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_HELPERS__
#define __UI_COMMAND_HELPERS__

#include <string>
#include <stdio.h>
#include <QString>
#include <iostream>
#include <FabricUI/Util/TypeInfo.h>

namespace FabricUI {
namespace Commands {

inline void printAndThrow(std::string error)
{
  printf("%s", error.c_str());
  throw(error.c_str());
}

inline QString parseTemplateCppType(
	const QString &type)
{
  int firstIndex = type.indexOf(
    "<"
    );

  int lastIndex = type.lastIndexOf(
    ">"
    );

  return type.mid(
    firstIndex+1,
    lastIndex-(firstIndex+1)
    );
}

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_HELPERS__
