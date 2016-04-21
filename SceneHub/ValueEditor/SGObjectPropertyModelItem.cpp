//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "SGObjectPropertyModelItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/SceneHub/Commands/SGSetPropertyCmd.h>

namespace FabricUI {
namespace SceneHub {

//////////////////////////////////////////////////////////////////////////
SGObjectPropertyModelItem::SGObjectPropertyModelItem(
  FabricCore::Client client,
  FabricCore::RTVal rtVal,
  bool isRootItem
  )
  : m_client( client )
  , m_rtVal( rtVal )
  , m_lastValueVersion(0)
  , m_rootItem( isRootItem )
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

FabricUI::ValueEditor::BaseModelItem *SGObjectPropertyModelItem::createChild( FTL::CStrRef name ) /**/
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
      if( m_rootItem )
        m_name = m_rtVal.callMethod( "String", "getFullPath", 0, 0 ).getStringCString();
      else
        m_name = m_rtVal.callMethod("String", "getName", 0, 0).getStringCString();
      return m_name;
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

FabricUI::ValueEditor::ItemMetadata* SGObjectPropertyModelItem::getMetadata()
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
    if( strcmp( m_rtValType.c_str(), "None" ) == 0 )
      m_rtValType = std::string();
  }
  catch(FabricCore::Exception e)
  {
    printf("SGObjectPropertyModelItem::getRTValType, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
  }

  return m_rtValType;
}

void SGObjectPropertyModelItem::updateFromScene() {
  if( !m_rtVal.isValid() )
    return;

  try {
    int valueVersion = m_rtVal.callMethod( "UInt32", "getValueVersion", 0, 0 ).getUInt32();
    if( m_lastValueVersion != valueVersion ) {
      // Value changed.
      m_lastValueVersion = valueVersion;

      // Update the type
      std::string prevType = m_rtValType;
      m_rtValType = std::string();
      getRTValType();
      if( prevType != m_rtValType ) {
        // TODO: notify that the type changed...
      }

      emitModelValueChanged( getValue() );
      // Check if type changed (todo!)
    }
  }
  catch( FabricCore::Exception e ) {
    printf( "SGObjectPropertyModelItem::updateFromScene, FabricCore::Exception: '%s'\n", e.getDesc_cstr() );
  }
}

QVariant SGObjectPropertyModelItem::getValue()
{
  FTL::CStrRef type = getRTValType();

  FabricCore::RTVal value;
  try
  {
    if(type == FTL_STR("Boolean"))
    {
      value = m_rtVal.callMethod("Boolean", "getBooleanValue", 0, 0);
    }
    else if(type == FTL_STR("UInt8"))
    {
      value = m_rtVal.callMethod("UInt8", "getUInt8Value", 0, 0);
    }
    else if(type == FTL_STR("SInt8"))
    {
      value = m_rtVal.callMethod("SInt8", "getSInt8Value", 0, 0);
    }
    else if(type == FTL_STR("UInt16"))
    {
      value = m_rtVal.callMethod("UInt16", "getUInt16Value", 0, 0);
    }
    else if(type == FTL_STR("SInt16"))
    {
      value = m_rtVal.callMethod("SInt16", "getSInt16Value", 0, 0);
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

      /* special case, convert to Xfo for editing */
      value = FabricCore::RTVal::Construct(m_client, "Xfo", 1, &value);
    }
    else if(type == FTL_STR("Color"))
    {
      value = m_rtVal.callMethod("Color", "getColorValue", 0, 0);
    }
    else if(type == FTL_STR("RGB"))
    {
      value = m_rtVal.callMethod("RGB", "getRGBValue", 0, 0);
    }
    else if(type == FTL_STR("RGBA"))
    {
      value = m_rtVal.callMethod("RGBA", "getRGBAValue", 0, 0);
    }
    else if(type == FTL_STR("ARGB"))
    {
      value = m_rtVal.callMethod("ARGB", "getARGBValue", 0, 0);
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

  if( !value.isValid() )
    return QVariant();// Might be another object type

  return toVariant(value);
}

void SGObjectPropertyModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  FabricCore::RTVal varVal;
  if(!isRTVal(var))
  {
    FTL::CStrRef type = getRTValType();

    try
    {
      if(type == FTL_STR("Boolean"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Boolean", 0, 0);
      }
      else if(type == FTL_STR("UInt8"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "UInt8", 0, 0);
      }
      else if(type == FTL_STR("SInt8"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "SInt8", 0, 0);
      }
      else if(type == FTL_STR("UInt16"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "UInt16", 0, 0);
      }
      else if(type == FTL_STR("SInt16"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "SInt16", 0, 0);
      }
      else if(type == FTL_STR("UInt32"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "UInt32", 0, 0);
      }
      else if(type == FTL_STR("SInt32"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "SInt32", 0, 0);
      }
      else if(type == FTL_STR("UInt64"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "UInt64", 0, 0);
      }
      else if(type == FTL_STR("SInt64"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "SInt64", 0, 0);
      }
      else if(type == FTL_STR("Float32"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Float32", 0, 0);
      }
      else if(type == FTL_STR("Float64"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Float64", 0, 0);
      }
      else if(type == FTL_STR("String"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "String", 0, 0);
      }
      else if(type == FTL_STR("Vec2"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Vec2", 0, 0);
      }
      else if(type == FTL_STR("Vec3"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Vec3", 0, 0);
      }
      else if(type == FTL_STR("Vec4"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Vec4", 0, 0);
      }
      else if(type == FTL_STR("Mat22"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Mat22", 0, 0);
      }
      else if(type == FTL_STR("Mat33"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Mat33", 0, 0);
      }
      else if(type == FTL_STR("Mat44"))
      {
        // special case!
        varVal = FabricCore::RTVal::Construct(m_client, "Xfo", 0, 0);
      }
      else if(type == FTL_STR("Color"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Color", 0, 0);
      }
      else if(type == FTL_STR("RGB"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "RGB", 0, 0);
      }
      else if(type == FTL_STR("RGBA"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "RGBA", 0, 0);
      }
      else if(type == FTL_STR("ARGB"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "ARGB", 0, 0);
      }
      else if(type == FTL_STR("Quat"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Quat", 0, 0);
      }
      else if(type == FTL_STR("Euler"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Euler", 0, 0);
      }
      else if(type == FTL_STR("Xfo"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Xfo", 0, 0);
      }
      else if(type == FTL_STR("Box2"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Box2", 0, 0);
      }
      else if(type == FTL_STR("Box3"))
      {
        varVal = FabricCore::RTVal::Construct(m_client, "Box3", 0, 0);
      }
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectPropertyModelItem::setValue, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
    if( !varVal.isValid() )
      return; //Might be another Object type
  }

  if(!FabricUI::ValueEditor::RTVariant::toRTVal(var, varVal))
    return;

  /* special case for Mat44 */
  if(getRTValType() == FTL_STR("Mat44"))
  {
    try
    {
      varVal = varVal.callMethod("Mat44", "toMat44", 0, 0);
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectPropertyModelItem::setValue, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }

  if(!m_rtVal.isValid())
    return;

  if (commit )
  {
    try
    {
      FabricCore::RTVal valueAtInteractionBeginVal;
      FabricUI::ValueEditor::RTVariant::toRTVal(valueAtInteractionBegin, valueAtInteractionBeginVal);

      /* special case for Mat44 */
      if(getRTValType() == FTL_STR("Mat44"))
        valueAtInteractionBeginVal = valueAtInteractionBeginVal.callMethod("Mat44", "toMat44", 0, 0);

      FabricCore::RTVal sgVal = m_rtVal.maybeGetMember("SG");
      QString fullPath( m_rtVal.callMethod("String", "getFullPath", 0, 0).getStringCString() );

      if(valueAtInteractionBeginVal.isValid())
        SGSetPropertyCmd cmd(m_client, sgVal, fullPath, valueAtInteractionBeginVal, varVal);
      else
        SGSetPropertyCmd cmd(m_client, sgVal, fullPath, varVal);

      emit synchronizeCommands();
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
