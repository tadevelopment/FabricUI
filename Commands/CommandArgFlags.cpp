//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandArgFlags.h"

/// Optional argument flag
int FabricUI::Commands::CommandArgFlags::OPTIONAL_ARG = 1;
/// Loggale argument flag
int FabricUI::Commands::CommandArgFlags::LOGGABLE_ARG = 2;
/// Input argument flag
int FabricUI::Commands::CommandArgFlags::IN_ARG = 4;
/// Output argument flag
int FabricUI::Commands::CommandArgFlags::OUT_ARG = 8;
/// IO argument flag
int FabricUI::Commands::CommandArgFlags::IO_ARG = 16;
