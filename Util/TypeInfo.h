//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_TYPE_INFO_UTIL__
#define __UI_TYPE_INFO_UTIL__

#include <QString>
#include <typeinfo>

namespace FabricUI {
namespace Util {

class TypeInfo
{
  public:
    TypeInfo() {}

    static QString demangle(
      const char* name
      );
};

template <class T> QString GetClassType(
  const T& t) 
{
  /**
    From http://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
    Use to unmangle the result of std::type_info::name
    
    Usage:
      #include <iostream>
      #include "<FabricUI/Util/TypeInfo.h>

      struct Base { virtual ~Base() {} };

      struct Derived : public Base { };

      int main() {

          Base* ptr_base = new Derived(); 

          std::cout << "Type of ptr_base: " 
                    << FabricUI::Util::type(ptr_base) 
                    << std::endl;

          std::cout << "Type of pointee: " 
                    << FabricUI::Util::type(*ptr_base) 
                    << std::endl;

          delete ptr_base;
      }
  */

  return TypeInfo::demangle(
    typeid(t).name()
    );
}

template <class T> QString GetTemplateClassType(
  const T& t)
{
  QString type = GetClassType(t);

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

} // namespace Util
} // namespace FabricUI

#endif // __UI_TYPE_INFO_UTIL__
