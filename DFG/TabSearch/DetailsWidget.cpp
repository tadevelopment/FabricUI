// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DetailsWidget.h"

#include "ItemView.h"

#include <FabricCore.h>
#include <iostream>
#include <QLayout>
#include <QLabel>
#include <QTextEdit>
#include <QVariant>
#include <QKeyEvent>
#include <FTL/JSONValue.h>

using namespace FabricUI::DFG::TabSearch;
using namespace FabricCore;

static const size_t ExecPortTypeNb = 3;
static const std::string ExecPortTypes[ExecPortTypeNb] = { "In", "IO", "Out" };

struct Port
{
  std::string type, name;
  std::string execPortType;
};

struct PresetDetails
{
  std::string description;
  std::vector<Port> ports;
  std::set<Query::Tag> tags;
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
          port.execPortType = portJs->getString( "execPortType" );
          details.ports.push_back( port );
        }
      }
      delete json;
    }
  }
  catch( const FTL::JSONException& e )
  {
    std::cerr << preset << "; Error : " << e.getDescCStr() << std::endl;
    assert( false );
  }

  details.tags = GetTags( preset, host );

  return details;
}

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

class DetailsWidget::Section : public QWidget
{
  void toggleCollapse( bool );

  struct Header : public Toggle
  {
    typedef Toggle Parent;

    Section *m_parent;
    Header( Section* parent, const std::string& text ) : m_parent( parent )
    {
      this->setObjectName( "Header" );
      QHBoxLayout* lay = new QHBoxLayout();
      QFrame* handle = new QFrame();
      handle->setObjectName( "Handle" );
      lay->addWidget( handle );
      QLabel* label = new QLabel( ToQString( text ) );
      label->setTextInteractionFlags( Qt::NoTextInteraction );
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
  DetailsWidget* m_parent;

public:

  class SectionWidget : public QWidget
  {
    Section* m_section;
  public:
    SectionWidget() : m_section( NULL ) {}
    void setSection( Section* s ) { m_section = s; }
  protected:
    void setSectionVisibility( bool visible ) { assert( m_section != NULL ); m_section->setVisible( visible ); }
  };

  Section( const std::string& name, DetailsWidget* parent )
    : m_header( new Header( this, name ) )
    , m_widget( NULL )
    , m_parent( parent )
  {
    this->setObjectName( "Section" );

    this->setLayout( new QVBoxLayout() );
    this->layout()->setMargin( 0 );
    this->layout()->setSpacing( 2 );
    this->layout()->addWidget( m_header );

    m_header->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
  }

  // takes ownership of the widget
  void setWidget( SectionWidget* widget )
  {
    if( m_widget != NULL )
    {
      this->layout()->removeWidget( m_widget );
      m_widget->deleteLater();
    }
    m_widget = widget;
    widget->setSection( this );
    this->layout()->addWidget( m_widget );
  }
};

void DetailsWidget::Section::toggleCollapse( bool toggled )
{
  if( m_widget != NULL )
    m_widget->setVisible( toggled );
  m_parent->updateSize();
}

class DetailsWidget::PortsView : public DetailsWidget::Section::SectionWidget
{
  struct PortView : public QWidget
  {
    PortView(
      const Port& port,
      const Query::Tag& tag = Query::Tag(),
      DetailsWidget* root = NULL
    )
    {
      this->setObjectName( "PortView" );
      QHBoxLayout* lay = new QHBoxLayout();
      lay->setMargin( 0 );
      lay->setSpacing( 1 );
      lay->addWidget( new QLabel( ToQString( port.name ) ) );
      if( tag == Query::Tag() )
        lay->addWidget( new Label( port.type ) );
      else
      {
        Label* label = new Label( port.type, tag );
        lay->addWidget( label );
        if( root != NULL )
          connect( label, SIGNAL( requestTag( const Query::Tag& ) ),
            root, SIGNAL( tagRequested( const Query::Tag& ) ) );
      }
      this->setLayout( lay );
    }
  };

  std::vector<PortView*> m_ports;

public:

  PortsView()
  {
    this->setObjectName( "PortsView" );
    QVBoxLayout* lay = new QVBoxLayout();
    lay->setSpacing( 1 );
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

  // Returns the tags it used
  std::set<Query::Tag> setPorts(
    const std::vector<Port>& ports,
    const std::set<Query::Tag>& tags,
    DetailsWidget* root
  )
  {
    clear();
    std::set<Query::Tag> usedTags;
    for( size_t i = 0; i < ports.size(); i++ )
    {
      Query::Tag tag( PortTypeCat, ports[i].type );
      if( tags.find( tag ) != tags.end() )
        usedTags.insert( tag );
      else
        tag = Query::Tag();
      PortView* view = new PortView( ports[i], tag, root );
      m_ports.push_back( view );
      this->layout()->addWidget( view );
    }

    setSectionVisibility( ports.size() > 0 );

    return usedTags;
  }
};

// http://doc.qt.io/qt-4.8/qt-layouts-flowlayout-example.html
// TODO : Knapsacks optimization problem to minimize the
// empty spaces ? https://en.wikipedia.org/wiki/Knapsack_problem
class DetailsWidget::TagContainer : public DetailsWidget::Section::SectionWidget
{
  struct Line
  {
    std::vector<TagWidget*> tags;
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
  DetailsWidget* m_preview;

public:

  TagContainer( DetailsWidget* preview )
    : m_layout( new QVBoxLayout() )
    , m_preview( preview )
  {
    this->setObjectName( "TagContainer" );

    this->setLayout( m_layout );
    m_layout->setSpacing( 2 );
    m_layout->setMargin( 8 );
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

  void setTags( const std::set<Query::Tag>& tags )
  {
    clear();

    for( std::set<Query::Tag>::const_iterator it = tags.begin(); it != tags.end(); it++ )
    {
      TagWidget* tagWidget = new TagWidget( *it );
      connect(
        tagWidget, SIGNAL( activated( const Query::Tag& ) ),
        m_preview, SIGNAL( tagRequested( const Query::Tag& ) )
      );
      if( m_lines.size() == 0
        || m_lines[m_lines.size() - 1].width() + tagWidget->sizeHint().width() > this->width() )
      {
        Line line;
        m_lines.push_back( line );
        m_layout->addLayout( line.layout );
      }
      Line& line = m_lines[m_lines.size() - 1];
      line.tags.push_back( tagWidget );
      line.layout->addWidget( tagWidget );
    }

    setSectionVisibility( tags.size() > 0 );
  }
};

void DetailsWidget::addSection( Section* s )
{
  this->layout()->addWidget( s );
  m_sections.push_back( s );
}

DetailsWidget::DetailsWidget( FabricCore::DFGHost* host )
  : m_host( host )
  , m_name( new Label() )
  , m_description( new Description() )
{
  this->setObjectName( "DetailsWidget" );

  m_name->setObjectName( "Name" );
  connect( m_name, SIGNAL( requestTag( const Query::Tag& ) ),
    this, SIGNAL( tagRequested( const Query::Tag& ) ) );
  m_description->setObjectName( "Description" );
  m_description->setAlignment( Qt::AlignTop );
  m_description->setReadOnly( true );
  m_description->setEnabled( false );
  m_description->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  m_name->setMinimumWidth( 300 );

  clear();
  QVBoxLayout* lay = new QVBoxLayout();
  lay->setSizeConstraint( QLayout::SetMinAndMaxSize );
  lay->setMargin( 0 );
  lay->setAlignment( Qt::AlignTop );
  this->setLayout( lay );

  lay->addWidget( m_name );
  lay->addWidget( m_description );

  Section* tags = new Section( "Tags", this );
  m_tagContainer = new TagContainer( this );
  tags->setWidget( m_tagContainer );
  this->addSection( tags );

  for( size_t i = 0; i < ExecPortTypeNb; i++ )
  {
    const std::string& execPortType = ExecPortTypes[i];
    Section* ports = new Section( execPortType + " Ports", this );
    PortsView* portsTable = new PortsView();
    ports->setWidget( portsTable );
    this->addSection( ports );
    m_portsTables.insert( PortsViews::value_type( execPortType, portsTable ) );
  }
}

void DetailsWidget::clear()
{
  m_preset = Result();
  m_name->set( "" );
}

void DetailsWidget::setPreset( const Result& preset )
{
  if( m_preset == preset )
    return;

  m_preset = preset;
  PresetDetails details = GetDetails( getPreset(), m_host );

  // Name
  {
    std::string name = m_preset.substr( m_preset.rfind( '.' ) + 1 );
    Query::Tag tag( NameCat, name );

    // Wrapping too long strings
    {
      int maxWidth = int( 0.8 * this->width() ); // Hack : harcoded margin value
      QFontMetrics nameMetrics = m_name->fontMetrics();
      QString txt = ToQString( name );
      QString wrapped;
      while( !txt.isEmpty() )
      {
        int i = 0;
        while( i < txt.size() && nameMetrics.width( txt, i ) < maxWidth )
          i++;
        wrapped += txt.left( i );
        if( i != txt.size() )
          wrapped += "-\n";
        txt = txt.right( txt.size() - i );
      }
      name = ToStdString( wrapped );
    }

    if( details.tags.find( tag ) != details.tags.end() )
    {
      m_name->set( name, tag );
      details.tags.erase( tag );
    }
    else
      m_name->set( name );
  }

  // Description
  m_description->setText( ToQString( details.description ) );
  m_description->setFixedHeight( int( m_description->document()->size().height() ) );
  m_description->setVisible( details.description.size() > 0 );

  // Ports
  {
    // Gathering Ports into ExecPortType categories
    typedef std::map<std::string, std::vector<Port> > PortMap;
    PortMap ports;
    for( std::vector<Port>::const_iterator it = details.ports.begin(); it != details.ports.end(); it++ )
      ports[it->execPortType].push_back( *it );

    // Adding each category to its section
    std::set<Query::Tag> unusedTags = details.tags;
    for( PortMap::const_iterator it = ports.begin(); it != ports.end(); it++ )
    {
      if( m_portsTables.find( it->first ) != m_portsTables.end() )
      {
        std::set<Query::Tag> usedTags =
          m_portsTables[it->first]->setPorts( it->second, details.tags, this );
        for( std::set<Query::Tag>::const_iterator it = usedTags.begin(); it != usedTags.end(); it++ )
          unusedTags.erase( *it );
      }
      else
      {
        std::cerr << "DetailsWidget::setPreset : Undefined ExecPortType " << it->first << std::endl;
        assert( false );
      }
    }
    details.tags = unusedTags;
  }

  // Filtering the Tags
  /*
  {
    std::set<Query::Tag> filtered;
    for( std::set<Query::Tag>::const_iterator it = details.tags.begin();
      it != details.tags.end(); it++ )
    {
      const Query::Tag& tag = *it;
      if(
        tag.cat() != PathCompCat &&
        tag.cat() != NameCompCat
      )
      {
        filtered.insert( tag );
      }
    }
    details.tags = filtered;
  }
  */

  // Tags
  m_tagContainer->setTags( details.tags );

  updateSize();
}

void DetailsWidget::updateSize()
{
  m_tagContainer->adjustSize();
  for( PortsViews::iterator it = m_portsTables.begin(); it != m_portsTables.end(); it++ )
    it->second->adjustSize();
  for( size_t i = 0; i < m_sections.size(); i++ )
    m_sections[i]->adjustSize();
  this->adjustSize();
}

const Result& DetailsWidget::getPreset() const { return m_preset; }
