//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "SGObjectPropertyModelItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/SceneHub/Commands/SGSetPropertyValueCmd.h>

namespace FabricUI {
namespace SceneHub {

//////////////////////////////////////////////////////////////////////////
SGObjectPropertyModelItem::SGObjectPropertyModelItem(
  SHCmdView * cmdView,
  FabricCore::Client client,
  FabricCore::RTVal rtVal
  )
  : m_cmdView( cmdView )
  , m_client( client )
  , m_rtVal( rtVal )
{
}

SGObjectPropertyModelItem::~SGObjectPropertyModelItem()
{
}


int SGObjectPropertyModelItem::getNumChildren()
{
  return 0; // todo
}

FTL::CStrRef SGObjectPropertyModelItem::getChildName( int i )
{
  return ""; // todo
}

BaseModelItem *SGObjectPropertyModelItem::createChild( FTL::CStrRef name ) /**/
{
  // return new ArgModelItem(
  //   m_dfgUICmdHandler,
  //   m_binding,
  //   name
  //   );
  return NULL; // todo
}

FTL::CStrRef SGObjectPropertyModelItem::getName()
{
  if(m_rtVal.isValid())
  {
    try
    {
      m_name = m_rtVal.callMethod("String", "getName", 0, 0).getStringCString();
      return m_name;;
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectPropertyModelItem::getName, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
  return FTL_STR("<Root>");
}

bool SGObjectPropertyModelItem::canRename()
{
  return false;
}

void SGObjectPropertyModelItem::rename( FTL::CStrRef newName )
{
}

void SGObjectPropertyModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
}

ItemMetadata* SGObjectPropertyModelItem::getMetadata()
{
  return NULL;
}

void SGObjectPropertyModelItem::setMetadataImp( 
  const char* key, 
  const char* value, 
  bool canUndo ) /**/
{
}

bool SGObjectPropertyModelItem::hasDefault()
{
  return false;
}

void SGObjectPropertyModelItem::resetToDefault()
{
}

FTL::CStrRef SGObjectPropertyModelItem::getRTValType()
{
  if(m_rtValType.length() > 0)
    return m_rtValType;

  if(!m_rtVal.isValid())
    return "";

  try
  {
    m_rtValType = m_rtVal.callMethod("String", "_getPropertyTypeAsString", 0, 0).getStringCString();
  }
  catch(FabricCore::Exception e)
  {
    printf("SGObjectPropertyModelItem::getRTValType, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
  }

  return m_rtValType;
}

QVariant SGObjectPropertyModelItem::getValue()
{
  FTL::CStrRef type = getRTValType();
  if(type.empty() || type == FTL_STR("Reference") || type == FTL_STR("Object"))
    return QVariant();

  FabricCore::RTVal value;
  try
  {
    if(type == FTL_STR("Boolean"))
    {
      value = m_rtVal.callMethod("Boolean", "getBooleanValue", 0, 0);
    }
    else if(type == FTL_STR("UInt32"))
    {
      value = m_rtVal.callMethod("UInt32", "getUInt32Value", 0, 0);
    }
    else if(type == FTL_STR("SInt32"))
    {
      value = m_rtVal.callMethod("SInt32", "getSInt32Value", 0, 0);
    }
    else if(type == FTL_STR("UInt64"))
    {
      value = m_rtVal.callMethod("UInt64", "getUInt64Value", 0, 0);
    }
    else if(type == FTL_STR("SInt64"))
    {
      value = m_rtVal.callMethod("SInt64", "getSInt64Value", 0, 0);
    }
    else if(type == FTL_STR("Float32"))
    {
      value = m_rtVal.callMethod("Float32", "getFloat32Value", 0, 0);
    }
    else if(type == FTL_STR("Float64"))
    {
      value = m_rtVal.callMethod("Float64", "getFloat64Value", 0, 0);
    }
    else if(type == FTL_STR("String"))
    {
      value = m_rtVal.callMethod("String", "getStringValue", 0, 0);
    }
    else if(type == FTL_STR("Vec2"))
    {
      value = m_rtVal.callMethod("Vec2", "getVec2Value", 0, 0);
    }
    else if(type == FTL_STR("Vec3"))
    {
      value = m_rtVal.callMethod("Vec3", "getVec3Value", 0, 0);
    }
    else if(type == FTL_STR("Vec4"))
    {
      value = m_rtVal.callMethod("Vec4", "getVec4Value", 0, 0);
    }
    else if(type == FTL_STR("Mat22"))
    {
      value = m_rtVal.callMethod("Mat22", "getMat22Value", 0, 0);
    }
    else if(type == FTL_STR("Mat33"))
    {
      value = m_rtVal.callMethod("Mat33", "getMat33Value", 0, 0);
    }
    else if(type == FTL_STR("Mat44"))
    {
      value = m_rtVal.callMethod("Mat44", "getMat44Value", 0, 0);
    }
    else if(type == FTL_STR("Color"))
    {
      value = m_rtVal.callMethod("Color", "getColorValue", 0, 0);
    }
    else if(type == FTL_STR("Quat"))
    {
      value = m_rtVal.callMethod("Quat", "getQuatValue", 0, 0);
    }
    else if(type == FTL_STR("Euler"))
    {
      value = m_rtVal.callMethod("Euler", "getEulerValue", 0, 0);
    }
    else if(type == FTL_STR("Xfo"))
    {
      value = m_rtVal.callMethod("Xfo", "getXfoValue", 0, 0);
    }
    else if(type == FTL_STR("Box2"))
    {
      value = m_rtVal.callMethod("Box2", "getBox2Value", 0, 0);
    }
    else if(type == FTL_STR("Box3"))
    {
      value = m_rtVal.callMethod("Box3", "getBox3Value", 0, 0);
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("SGObjectPropertyModelItem::getValue, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
  }
  return toVariant(value);
}

void SGObjectPropertyModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  if(!isRTVal(var))
    return;

  FabricCore::RTVal varVal;
  if(!RTVariant::toRTVal(var, varVal))
    return;

  if(!m_rtVal.isValid())
    return;

  if (commit && m_cmdView != NULL)
  {

    FabricCore::RTVal valueAtInteractionBeginVal;
    if(!RTVariant::toRTVal(valueAtInteractionBegin, valueAtInteractionBeginVal))
      return;

    try
    {
      std::vector<FabricCore::RTVal> params(6);
      params[0] = m_rtVal.callMethod("String", "getFullPath", 0, 0);
      params[1] = varVal.callMethod("Type", "type", 0, 0);
      params[2] = valueAtInteractionBeginVal.callMethod("Data", "data", 0, 0);
      params[3] = valueAtInteractionBeginVal.callMethod("UInt64", "dataSize", 0, 0);
      params[4] = varVal.callMethod("Data", "data", 0, 0);
      params[5] = varVal.callMethod("UInt64", "dataSize", 0, 0);

      FabricCore::RTVal shObject = m_cmdView->getShObject();
      SHCmd * cmd = new SGSetPropertyValueCmd(shObject, "SetValue", params, true);
      cmd->addRTValDependency(varVal);
      cmd->addRTValDependency(valueAtInteractionBeginVal);
      m_cmdView->addCommand(cmd);

      emitModelValueChanged(var);
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectPropertyModelItem::setValue, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
  else
  {
    try
    {
      m_rtVal.callMethod("", "setValue", 1, &varVal);
      emitModelValueChanged(var);
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectPropertyModelItem::setValue, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
}

} // namespace SceneHub
} // namespace FabricUI
