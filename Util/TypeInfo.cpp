//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "TypeInfo.h"

using namespace FabricUI;
using namespace Util;

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

struct handle 
{
  char* p;

  handle(
  	char* ptr) 
  : p(ptr) 
  { 
  }

  ~handle() 
  { 
  	std::free(p); 
  }
};

QString TypeInfo::demangle(
	const char* name) 
{
	// Set some arbitrary value to  
	// eliminate the compiler warning.
  int status = -4; 

  handle result( 
  	abi::__cxa_demangle(
	  	name, 
	  	NULL, 
	  	NULL, 
	  	&status
	  	) 
  	);

  return ( status == 0 ) 
  	? result.p 
  	: name;
}

#else

// does nothing if not g++
QString TypeInfo::demangle(
	const char* name) 
{
  return name;
}

#endif
