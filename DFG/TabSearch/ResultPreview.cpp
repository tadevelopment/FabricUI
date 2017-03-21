// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultPreview.h"

#include "ItemView.h"

#include <FabricCore.h>
#include "QueryEdit.h"
#include <iostream>
#include <QLayout>
#include <QLabel>
#include <QVariant>
#include <QKeyEvent>
#include <FTL/JSONValue.h>

using namespace FabricUI::DFG::TabSearch;
using namespace FabricCore;

struct Port
{
  std::string type, name;
};

struct PresetDetails
{
  std::string description;
  std::vector<Port> ports;
  std::vector<Query::Tag> tags;
};

PresetDetails GetDetails(
  const std::string& preset,
  DFGHost* host
)
{
  PresetDetails details;
  
  try
  {
    // Description of the DFGPreset
    {
      FabricCore::String descS = host->getPresetDesc( preset.data() );

      const FTL::JSONValue* json = FTL::JSONValue::Decode( descS.getCStr() );
      const FTL::JSONObject* presetJs = json->cast<FTL::JSONObject>();

      // Description
      if( presetJs->has( "metadata" ) )
      {
        const FTL::JSONObject* metadata = presetJs->getObject( "metadata" );
        if( metadata->has( "uiTooltip" ) )
          details.description = metadata->getString( "uiTooltip" );
      }

      // Ports
      if( presetJs->has( "ports" ) )
      {
        const FTL::JSONArray* ports = presetJs->getArray( "ports" );
        for( FTL::JSONArray::const_iterator it = ports->begin(); it != ports->end(); it++ )
        {
          const FTL::JSONObject* portJs = ( *it )->cast<FTL::JSONObject>();
          Port port;
          if( portJs->has( "typeSpec" ) )
            port.type = portJs->getString( "typeSpec" );
          port.name = portJs->getString( "name" );
          details.ports.push_back( port );
        }
      }
      delete json;
    }

    // Fetching tags from the DFGHost
    {
      FEC_StringRef tagsStrR = FEC_DFGHostGetPresetTags( host->getFECDFGHostRef(), preset.data() );
      FTL::StrRef tagsStr( FEC_StringGetCStr( tagsStrR ), FEC_StringGetSize( tagsStrR ) );
      FTL::JSONValue* tags = FTL::JSONValue::Decode( tagsStr );
      FTL::JSONArray* tagsA = tags->cast<FTL::JSONArray>();

      for( FTL::JSONArray::const_iterator it = tagsA->begin(); it != tagsA->end(); it++ )
        details.tags.push_back( std::string( ( *it )->getStringValue() ) );

      delete tags;
    }
  }
  catch( const FTL::JSONException& e )
  {
    std::cerr << preset << "; Error : " << e.getDescCStr() << std::endl;
    assert( false );
  }

  return details;
}

inline QString Bold( const QString& s ) { return "<b>" + s + "</b>"; }

Toggle::Toggle( bool toggled )
{
  setHovered( false );
  setToggled( false );
}

void Toggle::setToggled( bool t )
{
  m_toggled = t;
  emit toggled( m_toggled );
  this->setProperty( "toggled", QVariant( m_toggled ) );
  this->setStyleSheet( this->styleSheet() );
}

void Toggle::setHovered( bool h )
{
  m_hovered = h;
  this->setProperty( "hovered", QVariant( m_hovered ) );
  this->setStyleSheet( this->styleSheet() );
}

void Toggle::mouseReleaseEvent( QMouseEvent* e )
{
  Parent::mouseReleaseEvent( e );
  setToggled( !m_toggled );
}

void Toggle::enterEvent( QEvent* e )
{
  Parent::enterEvent( e );
  setHovered( true );
}

void Toggle::leaveEvent( QEvent* e )
{
  Parent::leaveEvent( e );
  setHovered( false );
}

class Section : public QWidget
{
  void toggleCollapse( bool );

  struct Header : public Toggle
  {
    typedef Toggle Parent;

    Section *m_parent;
    Header( Section* parent, const std::string& text ) : m_parent( parent )
    {
      QHBoxLayout* lay = new QHBoxLayout();
      QFrame* handle = new QFrame();
      handle->setObjectName( "Handle" );
      lay->addWidget( handle );
      QLabel* label = new QLabel( Bold( QString::fromStdString( text ) ) );
      label->setTextInteractionFlags( Qt::TextInteractionFlag::NoTextInteraction );
      label->setObjectName( "Name" );
      label->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum ) );
      lay->addWidget( label );
      lay->setAlignment( Qt::AlignLeft );
      lay->setMargin( 0 );
      lay->setSpacing( 4 );
      this->setLayout( lay );
      this->setToggled( true );
    }

    void mouseReleaseEvent( QMouseEvent *ev ) FTL_OVERRIDE
    {
      Parent::mouseReleaseEvent( ev );
      m_parent->toggleCollapse( isToggled() );
    }
  };

  Header* m_header;
  QWidget* m_widget;
  ResultPreview* m_parent;

public:
  Section( const std::string& name, ResultPreview* parent )
    : m_header( new Header( this, name ) )
    , m_widget( NULL )
    , m_parent( parent )
  {
    this->setObjectName( "Section" );
    m_header->setObjectName( "Header" );

    this->setLayout( new QVBoxLayout() );
    this->layout()->setMargin( 0 );
    this->layout()->setSpacing( 2 );
    this->layout()->addWidget( m_header );

    m_header->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
  }

  // takes ownership of the widget
  void setWidget( QWidget* widget )
  {
    if( m_widget != NULL )
    {
      this->layout()->removeWidget( m_widget );
      m_widget->deleteLater();
    }
    m_widget = widget;
    this->layout()->addWidget( m_widget );
  }
};

void Section::toggleCollapse( bool toggled )
{
  if( m_widget != NULL )
    m_widget->setVisible( toggled );
  m_parent->adjustSize();
}

class ResultPreview::PortsView : public QWidget
{
  struct PortView : public QWidget
  {
    PortView( const Port& port )
    {
      this->setObjectName( "PortView" );
      QHBoxLayout* lay = new QHBoxLayout();
      lay->setMargin( 0 );
      lay->setSpacing( 2 );
      lay->addWidget( new QLabel( QString::fromStdString( port.name ) ) );
      lay->addWidget( new QLabel( QString::fromStdString( port.type ) ) );
      this->setLayout( lay );
    }
  };

  std::vector<PortView*> m_ports;

public:

  PortsView()
  {
    this->setObjectName( "PortsView" );
    QVBoxLayout* lay = new QVBoxLayout();
    lay->setMargin( 0 );
    lay->setSpacing( 2 );
    lay->setMargin( 8 );
    this->setLayout( lay );

    Port headerPort;
    headerPort.name = "Name";
    headerPort.type = "Type";
    PortView* header = new PortView( headerPort );
    header->setObjectName( "PortsHeader" );
    this->layout()->addWidget( header );
  }

  void clear()
  {
    for( size_t i = 0; i < m_ports.size(); i++ )
    {
      this->layout()->removeWidget( m_ports[i] );
      m_ports[i]->deleteLater();
    }
    m_ports.clear();
  }

  void setPorts( const std::vector<Port>& ports )
  {
    clear();
    for( size_t i = 0; i < ports.size(); i++ )
    {
      PortView* view = new PortView( ports[i] );
      m_ports.push_back( view );
      this->layout()->addWidget( view );
    }
  }
};

// http://doc.qt.io/qt-4.8/qt-layouts-flowlayout-example.html
// TODO : Knapsacks optimization problem to minimize the
// empty spaces ? https://en.wikipedia.org/wiki/Knapsack_problem
class ResultPreview::TagsView : public QWidget
{
  struct Line
  {
    std::vector<TagView*> tags;
    QHBoxLayout* layout;
    Line() : layout( new QHBoxLayout() ) {
      layout->setAlignment( Qt::AlignLeft );
      layout->setSpacing( 2 );
      layout->setMargin( 0 );
    }
    int width() const
    {
      int w = 0;
      for( size_t i = 0; i < tags.size(); i++ )
        w += tags[i]->sizeHint().width() + layout->spacing();
      return w;
    }
  };
  std::vector<Line> m_lines;
  QVBoxLayout* m_layout;
  ResultPreview* m_preview;

public:

  TagsView( ResultPreview* preview )
    : m_layout( new QVBoxLayout() )
    , m_preview( preview )
  {
    this->setObjectName( "TagsView" );

    this->setLayout( m_layout );
    m_layout->setSpacing( 2 );
    m_layout->setMargin( 4 );
  }

  void clear()
  {
    for( size_t i = 0; i < m_lines.size(); i++ )
    {
      const Line& line = m_lines[i];
      m_layout->removeItem( line.layout );
      for( size_t j = 0; j < line.tags.size(); j++ )
        line.tags[j]->deleteLater();
      line.layout->deleteLater();
    }
    m_lines.clear();
  }

  void setTags( const std::vector<Query::Tag>& tags )
  {
    clear();

    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagView* tagView = new TagView( tags[i] );
      connect(
        tagView, SIGNAL( activated( const std::string& ) ),
        m_preview, SIGNAL( tagRequested( const std::string& ) )
      );
      if( m_lines.size() == 0
        || m_lines[m_lines.size() - 1].width() + tagView->sizeHint().width() > this->width() )
      {
        Line line;
        m_lines.push_back( line );
        m_layout->addLayout( line.layout );
      }
      Line& line = m_lines[m_lines.size() - 1];
      line.tags.push_back( tagView );
      line.layout->addWidget( tagView );
    }
  }
};

ResultPreview::ResultPreview( FabricCore::DFGHost* host )
  : m_host( host )
  , m_name( new QLabel() )
  , m_description( new QLabel() )
{
  this->setObjectName( "ResultPreview" );
  m_name->setObjectName( "Name" );
  m_description->setObjectName( "Description" );

  clear();
  QVBoxLayout* lay = new QVBoxLayout();
  lay->setMargin( 0 );
  lay->setAlignment( Qt::AlignTop );
  this->setLayout( lay );

  lay->addWidget( m_name );
  lay->addWidget( m_description );
  m_description->setWordWrap( true );

  Section* tags = new Section( "Tags", this );
  m_tagsView = new TagsView( this );
  tags->setWidget( m_tagsView );
  lay->addWidget( tags );

  Section* ports = new Section( "Ports", this );
  m_portsTable = new PortsView();
  ports->setWidget( m_portsTable );
  lay->addWidget( ports );
}

void ResultPreview::clear()
{
  m_preset = "";
  m_name->setText( "" );
}

void ResultPreview::setPreset( const std::string& preset )
{
  if( m_preset == preset )
    return;

  m_preset = preset;
  std::string name = m_preset.substr( m_preset.rfind( '.' ) + 1 );
  m_name->setText( Bold( QString::fromStdString( name ) ) );

  PresetDetails details = GetDetails( getPreset(), m_host );
  m_description->setText( QString::fromStdString( details.description ) );
  m_description->setVisible( !details.description.empty() );

  m_tagsView->setTags( details.tags );

  m_portsTable->setPorts( details.ports );
  
  this->adjustSize();
}

const std::string& ResultPreview::getPreset() const { return m_preset; }
