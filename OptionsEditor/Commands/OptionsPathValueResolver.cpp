// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QStringList>
#include "../OptionsEditorHelpers.h"
#include <FabricUI/Util/RTValUtil.h>
#include "OptionsPathValueResolver.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace Application;
using namespace OptionsEditor;

OptionsPathValueResolver::OptionsPathValueResolver()
  : BasePathValueResolver()
{
}

OptionsPathValueResolver::~OptionsPathValueResolver()
{
}

bool OptionsPathValueResolver::knownPath(
  RTVal pathValue)
{
  bool hasOption = false;

  try 
  {
    pathValue = RTValUtil::toRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    int index = path.indexOf("/");
    
    RTVal id = RTVal::ConstructString(
      pathValue.getContext(),
      path.midRef(0, index).toUtf8().constData()
      );

    RTVal optionsTarget = GetKLOptionsTargetRegistry().callMethod(
      "OptionsTarget",
      "getTarget",
      1,
      &id);

    hasOption = optionsTarget.isValid() && !optionsTarget.isNullObject();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "OptionsPathValueResolver::knownPath",
      "",
      e.getDesc_cstr());
  }

  return hasOption;
}

QString OptionsPathValueResolver::getType(
  RTVal pathValue)
{
  QString portType;

  try 
  {
    getValue(pathValue);

    portType = RTValUtil::getRTValType(
      RTValUtil::toRTVal(pathValue.maybeGetMember("value"))
      );
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "OptionsPathValueResolver::getType",
      "",
      e.getDesc_cstr());
  }

  return portType;
}

void OptionsPathValueResolver::getValue(
  RTVal pathValue)
{
  try 
  {
    pathValue = RTValUtil::toRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    RTVal singleOption = GetKLOptionsTargetSingleOption(
      path);

    pathValue.setMember(
      "value", 
      singleOption);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "OptionsPathValueResolver::getValue",
      "",
      e.getDesc_cstr()
      );
  }
}

void OptionsPathValueResolver::setValue(
  RTVal pathValue)
{
  try
  {
    pathValue = RTValUtil::toRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();
    
    RTVal option = pathValue.maybeGetMember(
      "value");
 
    SetKLOptionsTargetSingleOption(
      path,
      option);
    
    int index = path.indexOf("/");
    
    GetOptionsEditor(
      path.midRef(0, index).toUtf8().constData()
      )->updateModel();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "OptionsPathValueResolver::setValue",
      "",
      e.getDesc_cstr());
  }
}
