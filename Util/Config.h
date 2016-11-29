// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_Config__
#define __UI_Config__

#include <FTL/Config.h>
#include <FTL/JSONValue.h>

#include <FabricUI/Util/FabricResourcePath.h>

namespace FabricUI
{
  namespace Util
  {

    class ConfigSection
    {
      template<typename T>
      T getValue( const FTL::JSONValue* entry ) const;

      template<typename T>
      FTL::JSONValue* createValue( const T defaultValue ) const;

    public:
      ConfigSection& getOrCreateSection( const std::string name );

      template <typename T>
      T getOrCreateValue( const std::string key, const T defaultValue )
      {
        if ( !m_json->has( key ) )
        {
          m_json->insert( key, createValue<T>( defaultValue ) );
          return defaultValue;
        }
        try
        {
          return getValue<T>( m_json->get( key ) );
        }
        catch ( FTL::JSONException e )
        {
          printf(
            "Error : malformed entry for key \"%s\" : \"%s\"\n",
            key.data(),
            m_json->get( key )->encode().data()
          );
          return defaultValue;
        }
      }

    protected:
      std::map<std::string, ConfigSection> m_sections;
      FTL::JSONObject* m_json;
    };

    class Config : public ConfigSection
    {
    public:
      Config( const std::string fileName = FabricResourcePath( "config.json" ) );
      ~Config();

    private:
      std::string m_fileName;
      std::string m_content;
    };

  }
}

#endif // __UI_Config__
