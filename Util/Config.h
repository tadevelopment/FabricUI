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

      template <typename T> // TODO : remove this function
      T _getOrCreateValue( const QString key, const T defaultValue = T() );

    public:
      ConfigSection& getOrCreateSection( const QString name );

      template <typename T>
      T getOrCreateValue( const QString key, const T defaultValue = T() );

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
