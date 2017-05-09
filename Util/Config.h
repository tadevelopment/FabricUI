// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_Config__
#define __UI_Config__

#include <FTL/Config.h>
#include <FTL/JSONValue.h>
#include <FTL/SharedPtr.h>

#include <QColor>
#include <QFont>
#include <QString>

#include <map>

namespace FabricUI
{
  namespace Util
  {

    // A ConfigSection contains values and other ConfigSections, each
    // one associated with a String key
    class ConfigSection : public FTL::Shareable
    {
      template<typename T>
      T getValue( const FTL::JSONValue* entry ) const;

      template<typename T>
      FTL::JSONValue* createValue( const T defaultValue ) const;

    public:

      enum Access
      {
        ReadOnly,
        WriteOnly,
        ReadWrite
      };

      ConfigSection()
        : m_json( NULL )
        , m_previousSection( NULL )
        , m_access( ReadWrite )
      {}
      virtual ~ConfigSection() {};

      ConfigSection& getOrCreateSection( const FTL::StrRef name );

      template <typename T>
      T getOrCreateValue( const FTL::StrRef key, const T defaultValue )
      {
        if( getAccess() == WriteOnly || !m_json->has( key ) )
        {
          // if the key is not there, and there is a previous section, query it
          if ( m_previousSection != NULL )
            return m_previousSection->getOrCreateValue( key, defaultValue );

          // else, insert the default value in this section
          if( getAccess() != ReadOnly )
            m_json->insert( key, createValue<T>( defaultValue ) );
          return defaultValue;
          ;
        }
        try
        {
          // if the key is there, try to return it
          return getValue<T>( m_json->get( key ) );
        }
        catch ( FTL::JSONException e )
        {
          printf(
            "Error : malformed entry for key \"%s\" : \"%s\"\n",
            key.data(),
            m_json->get( key )->encode().data()
          );
          // if the value is malformed, either query the previous
          // section or return the default value
          return m_previousSection != NULL ?
            m_previousSection->getOrCreateValue( key, defaultValue ) :
            defaultValue
          ;
        }
      }

      // Used by shiboken
#define DECLARE_EXPLICIT_GETTER( T, method ) \
      inline T  method( const FTL::StrRef key, const T defaultValue ) \
        { return getOrCreateValue<T>( key, defaultValue ); }

      DECLARE_EXPLICIT_GETTER( bool, getOrCreateBool )
      DECLARE_EXPLICIT_GETTER( int, getOrCreateInt )
      DECLARE_EXPLICIT_GETTER( unsigned int, getOrCreateUInt )
      DECLARE_EXPLICIT_GETTER( double, getOrCreateDouble )
      DECLARE_EXPLICIT_GETTER( float, getOrCreateFloat )
      DECLARE_EXPLICIT_GETTER( QString, getOrCreateQString )
      DECLARE_EXPLICIT_GETTER( QColor, getOrCreateQColor )
      DECLARE_EXPLICIT_GETTER( QFont, getOrCreateQFont )

#undef DECLARE_EXPLICIT_GETTER

      void setAccess( const Access access );
      Access getAccess() const { return m_access; }

    protected:
      std::map<std::string, FTL::SharedPtr<ConfigSection> > m_sections;
      FTL::JSONObject* m_json;
      // Config to look into if a value is not found here
      ConfigSection* m_previousSection;
      Access m_access;
    };

    // A Config is a root ConfigSection, associated with a file on the disk
    class Config : public ConfigSection
    {
      void open( const FTL::StrRef fileName );
      Config( const FTL::StrRef fileName, Access access = ReadWrite );

    public:
      Config();
      ~Config();

    private:
      std::string m_fileName;
      std::string m_content;
    };

  }
}

#endif // __UI_Config__
