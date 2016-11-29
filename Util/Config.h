// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_Config__
#define __UI_Config__

#include <FTL/Config.h>
#include <FTL/JSONValue.h>

#include <FabricUI/Util/FabricResourcePath.h>

#include <QMap>
#include <QString>

using namespace FTL;

namespace FabricUI
{
  namespace Util
  {

    class ConfigSection
    {
      template<typename T>
      T getValue( const JSONValue* entry ) const;

      template<typename T>
      JSONValue* createValue( const T defaultValue ) const;

    public:
      ConfigSection& getOrCreateSection( const QString name );

      template <typename T>
      T getOrCreateValue( const QString key, const T defaultValue )
      {
        if ( !m_json->has( key.toStdString() ) )
        {
          m_json->insert( key.toStdString(), createValue<T>( defaultValue ) );
          return defaultValue;
        }
        try
        {
          return getValue<T>( m_json->get( key.toStdString() ) );
        }
        catch ( FTL::JSONException e )
        {
          printf(
            "Error : malformed entry for key \"%s\" : \"%s\"\n",
            key.toStdString(),
            m_json->get( key.toStdString() )->encode().data()
          );
          return defaultValue;
        }
      }

    protected:
      QMap<QString, ConfigSection> m_sections;
      JSONObject* m_json;
    };

    class Config : public ConfigSection
    {
    public:
      Config( const QString fileName = FabricResourcePathQS( "config.json" ) );
      ~Config();

    private:
      QString m_fileName;
      QString m_content;
    };

  }
}

#endif // __UI_Config__
