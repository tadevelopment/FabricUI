// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

#include "ItemView.h"

#include <QLayout>

#include <FabricCore.h>
#include <FTL/JSONValue.h>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

struct JSONSerializable
{
  virtual FTL::JSONValue* toJSON() const = 0;
  virtual ~JSONSerializable() {}
};

struct NameAndScore : JSONSerializable
{
  std::string name;
  double score;
  NameAndScore() {}
  NameAndScore( const std::string& name, const double& score )
    : name( name ), score( score ) {}
  QString toString() const { return QString::fromStdString( name ) + " [" + QString::number( score ) + "]"; }
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    FTL::JSONObject* obj = new FTL::JSONObject();
    obj->insert( "name", new FTL::JSONString( name ) );
    obj->insert( "score", new FTL::JSONFloat64( score ) );
    return obj;
  }
};

struct Tag : NameAndScore
{
  Tag() {}
  Tag( const std::string& name, const double& score ) : NameAndScore( name, score ) {}
};

struct Preset : NameAndScore
{
  Preset() {}
  Preset( const std::string& name, const double& score ) : NameAndScore( name, score ) {}
};

struct Tags : public std::vector<Tag>, public JSONSerializable
{
  inline void operator+=( const Tag& t )
  { this->push_back( t ); }

  QString toString() const
  {
    QString dst;
    for( const_iterator it = begin(); it < end(); it++ )
      dst += it->toString() + " ";
    return dst;
  }
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    FTL::JSONArray* ar = new FTL::JSONArray();
    for( const_iterator it = begin(); it < end(); it++ )
      ar->push_back( it->toJSON() );
    return ar;
  }
};

// Variant : can be either a Preset, or T
template<typename T>
class PresetOr : JSONSerializable
{
  enum Type { UNDEFINED, PRESET, OTHER } type;
  Preset preset;
  T other;

public:
  PresetOr() : type( UNDEFINED ) {}
  PresetOr( const Preset& p )
    : type( PRESET ), preset( p )
  {}
  PresetOr( const T& o )
    : type( OTHER ), other( o )
  {}
  template<typename T2>
  void assign( const T2& o )
  {
    this->type = o.type;
    this->preset = o.preset;
    this->other = o.other;
  }
  inline bool isUndefined() const { return type == UNDEFINED; }
  inline bool isPreset() const { return type == PRESET; }
  inline bool isOther() const { return type == OTHER; }
  inline Preset& getPreset()
  {
    assert( type == PRESET );
    return preset;
  }
  inline const Preset& getPreset() const
  {
    assert( type == PRESET );
    return preset;
  }
  inline T& getOther()
  {
    assert( type == OTHER );
    return other;
  }
  inline const T& getOther() const
  {
    assert( type == OTHER );
    return other;
  }
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    if( type == UNDEFINED )
      return new FTL::JSONString( "undefined" );
    FTL::JSONObject* obj = new FTL::JSONObject();
    obj->insert( "isPreset", new FTL::JSONBoolean( this->isPreset() ) );
    obj->insert( "value", ( isPreset() ? getPreset().toJSON() : getOther().toJSON() ) );
    return obj;
  }
};

// Generic tree structure, used below
template<typename T>
struct Node : JSONSerializable
{
  T value;
  typedef std::vector<Node> Children;
  Children children;

  std::string toEncodedJSON() const
  {
    FTL::JSONValue* json = this->toJSON();
    std::string dst = json->encode();
    delete json;
    return dst;
  }

protected:
  // Utilitary method to convert a Tree to another
  template<typename T2>
  void assign(const Node<T2>& o)
  {
    this->value.assign( o.value );
    this->children.resize( o.children.size() );
    for( size_t i=0; i<o.children.size(); i++ )
      this->children[i].assign( o.children[i] );
  }
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    FTL::JSONObject* obj = new FTL::JSONObject();
    obj->insert( "value", value.toJSON() );
    FTL::JSONArray* ar = new FTL::JSONArray();
    for( size_t i=0; i<children.size(); i++ )
      ar->push_back( children[i].toJSON() );
    obj->insert( "children", ar );
    return obj;
  }
};

typedef std::map<std::string, size_t> Indexes;
struct TagAndMap : JSONSerializable
{
  Tag tag;
  // Map from the name of to the index of a child
  Indexes tagIndexes;
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    // TODO
    return tag.toJSON();
  }
};

// First step : Temporary tree used to gather results by Tag
typedef Node< PresetOr< TagAndMap > > TmpNode;
TmpNode& AddTag( TmpNode& t, const Tag& tag )
{
  const std::string& key = tag.name;
  if( t.value.isUndefined() )
    t.value = TagAndMap();
  Indexes& indexes = t.value.getOther().tagIndexes;
  if( indexes.find( key ) == indexes.end() )
  {
    // If this is a new tag, add it as a child
    indexes.insert( Indexes::value_type( key, t.children.size() ) );
    TagAndMap tam; tam.tag = tag;
    TmpNode newItem; newItem.value = tam;
    t.children.push_back( newItem );
  }
  return t.children[indexes[key]];
}

// Second step : Reducing the tree by fusioning consecutive nodes
struct ReducedNode : Node< PresetOr< Tags > >
{
  ReducedNode( const TmpNode& tmpTI )
  {
    if( !tmpTI.value.isPreset() )
    {
      const TmpNode* n = &tmpTI;
      if( this->value.isUndefined() )
        this->value = Tags();
      this->value.getOther() += n->value.getOther().tag;
      // If there is a chain of items (Tag or Preset) with
      // only one child, merge them into a single Item
      while( n->children.size() == 1 )
      {
        n = &n->children[0];
        if( !n->value.isPreset() )
          this->value.getOther() += n->value.getOther().tag;
      }
      // If the child is a preset : override all the tags by
      // that single preset
      if( n->value.isPreset() )
        value = PresetOr<Tags>( n->value .getPreset() );
      else
      {
        // otherwise, just add all its children
        for( std::vector<TmpNode>::const_iterator it = n->children.begin();
          it != n->children.end(); it++ )
        {
          this->children.push_back( ReducedNode( *it ) );
        }
      }
    }
    else
      value = PresetOr<Tags>( tmpTI.value.getPreset() );
  }
};

// Third and last step : this tree is the one
// used by the Qt model
struct ModelNode;
struct ModelValue : PresetOr<Tags>
{
  // Pointers to the parents : this is required
  // by the QAbstractItemModel
  ModelNode* parent;
  size_t index;

  QString toString() const
  {
    if( isPreset() )
      return getPreset().toString();
    else
      return getOther().toString();
  }
};

struct ModelNode : Node<ModelValue>
{
  // TODO : check that parent pointers have been computed
  ModelNode() {}
  ModelNode( const ReducedNode& o ) { this->assign( o ); }
};
void ComputeParents( ModelNode& item )
{
  for( size_t i = 0; i<item.children.size(); i++ )
  {
    item.children[i].value.parent = &item;
    item.children[i].value.index = i;
    ComputeParents( static_cast<ModelNode&>( item.children[i] ) );
  }
}

TmpNode BuildResultTree( const std::string& searchResult )
{
  const FTL::JSONValue* json = FTL::JSONValue::Decode( searchResult.c_str() );
  const FTL::JSONObject* root = json->cast<FTL::JSONObject>();
  const FTL::JSONArray* resultsJson = root->getArray( "results" );

  TmpNode rootNode;
  for( size_t i = 0; i < resultsJson->size(); i++ )
  {
    const FTL::JSONArray* result = resultsJson->getArray( i );
    const FTL::JSONArray* tags = result->getArray( 2 );
    TmpNode* node = &rootNode;
    for( size_t j = 0; j < tags->size(); j++ )
    {
      const FTL::JSONObject* tagO = tags->getObject( j );
      node = &AddTag( *node, Tag(
        tagO->getString( "tag" ),
        tagO->getFloat64( "weight" )
      ) );
    }
    TmpNode newItem; newItem.value = Preset(
      result->getString( 0 ),
      result->getFloat64( 1 )
    );
    node->children.push_back( newItem );
  }
  return rootNode;
}

namespace Test
{
  static void ReportCallBack(
    void *userdata,
    FEC_ReportSource source,
    FEC_ReportLevel level,
    char const *data,
    uint32_t size
  )
  {
    std::cout << std::string( data, size ).c_str() << std::endl;
  }

  template<typename NodeT>
  size_t CountPresets( const NodeT& n )
  {
    if( n.value.isPreset() )
      return 1;
    size_t sum = 0;
    for( size_t i = 0; i < n.children.size(); i++ )
      sum += CountPresets( n.children[i] );
    return sum;
  }

  void Write( const std::string& filePath, const std::string& content )
  {
    std::ofstream file( filePath.data() );
    file << content;
  }

  template<typename NodeT>
  size_t LogTree( const NodeT& root, const std::string& fileName )
  {
    size_t presetCount = CountPresets( root );
    std::cout << presetCount << " presets in " << fileName << std::endl;
    Write( fileName, root.toEncodedJSON() );
    return presetCount;
  }
}

void ResultsView::UnitTest( const std::string& logFolder )
{
  // Core Client
  FabricCore::Client::CreateOptions createOptions = {};
  createOptions.guarded = true;
  FabricCore::Client client( &Test::ReportCallBack, 0, &createOptions );
  FabricCore::DFGHost host = client.getDFGHost();

  const char* searchTerms[2] = { "Get", "Sphere" };

  size_t originalCount = 32;
  FEC_StringRef jsonStr = FEC_DFGHostSearchPresets(
    host.getFECDFGHostRef(),
    2,
    searchTerms,
    0,
    NULL,
    0,
    originalCount
  );
  FTL::StrRef jsonStrR( FEC_StringGetCStr( jsonStr ), FEC_StringGetSize( jsonStr ) );
  std::string json = jsonStrR;
  Test::Write( logFolder + "0_results.json", json );

  TmpNode tmpNode = BuildResultTree( json );
  size_t newCount = Test::LogTree( tmpNode, logFolder + "1_tmpNode.json" );
  assert( newCount == originalCount );

  ReducedNode redNode = tmpNode;
  newCount = Test::LogTree( redNode, logFolder + "2_reduced.json" );
  assert( newCount == originalCount );

  ModelNode modNode = redNode;
  newCount = Test::LogTree( modNode, logFolder + "3_final.json" );
  assert( newCount == originalCount );
}

// Model for the TreeView
class ResultsView::Model : public QAbstractItemModel
{

private:
  ModelNode root;

  inline const ModelNode* cast( const QModelIndex& index ) const
  { return static_cast<ModelNode*>( index.internalPointer() ); }

public:

  inline bool isPreset( const QModelIndex& index ) const
  { return cast( index )->value.isPreset(); }

  const Preset& getPreset( const QModelIndex& index ) const
  { return cast( index )->value.getPreset(); }

  const Tags& getTags( const QModelIndex& index ) const
  { return cast( index )->value.getOther(); }

  QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const FTL_OVERRIDE
  {
    if( !hasIndex( row, column, parent ) )
      return QModelIndex();
    const ModelNode* parentItem = ( parent.isValid() ? cast( parent ) : &this->root );
    return ( row < int( parentItem->children.size() ) ?
      this->createIndex( row, column, ( void* ) &( parentItem->children[row] ) )
      : QModelIndex() );
  }
  QModelIndex parent( const QModelIndex & child ) const FTL_OVERRIDE
  {
    if( !child.isValid() )
      return QModelIndex();
    const ModelNode* parent = cast( child )->value.parent;
    return ( parent == &root ? QModelIndex() : createIndex( parent->value.index, 0, (void*)parent ) );
  }
  int rowCount( const QModelIndex & parent = QModelIndex() ) const FTL_OVERRIDE
  {
    return ( parent.isValid() ? cast( parent )->children.size() : root.children.size() );
  }
  int columnCount( const QModelIndex & parent = QModelIndex() ) const FTL_OVERRIDE { return 1; }
  QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const FTL_OVERRIDE
  {
    //return ( index.isValid() && role == Qt::DisplayRole ?
    //  cast( index )->value.toString() : QVariant() );
    return QVariant();
  }

  void setRoot( const TmpNode& root ) {
    this->beginResetModel();
    // Performing the 3 steps here (by converting each type)
    this->root = ReducedNode( root );
    ComputeParents( this->root );
    this->endResetModel();
  }
};

ResultsView::ResultsView()
  : m_model( new Model() )
{
  setItemsExpandable( false );
  setModel( m_model );
  this->setHeaderHidden( true );
  connect(
    this->selectionModel(), SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
    this, SLOT( onSelectionChanged() )
  );
  connect(
    this, SIGNAL( doubleClicked( const QModelIndex & ) ),
    this, SLOT( validateSelection() )
  );
  this->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

ResultsView::~ResultsView()
{
  delete m_model;
}

void ResultsView::validateSelection()
{
  if( m_model->isPreset( currentIndex() ) )
    emit presetValidated( this->getSelectedPreset() );
}

void ResultsView::onSelectionChanged()
{
  if( m_model->isPreset( currentIndex() ) )
    emit presetSelected( this->getSelectedPreset() );
  else
    emit presetDeselected();
}

void ResultsView::setResults( const std::string& searchResult )
{
  m_model->setRoot( BuildResultTree( searchResult ) );
  this->expandAll();

  if( m_model->rowCount() > 0 )
  {
    const QModelIndex& firstEntry = m_model->index( 0, 0 );
    if( m_model->isPreset( firstEntry ) )
      this->setCurrentIndex( firstEntry );
  }
  replaceViewItems();
}

QString ResultsView::getSelectedPreset()
{
  assert( m_model->isPreset( currentIndex() ) );
  return QString::fromStdString( m_model->getPreset( currentIndex() ).name );
}

void ResultsView::keyPressEvent( QKeyEvent * event )
{
  Parent::keyPressEvent( event );
}

struct TagsView : public QWidget
{
  TagsView( const Tags& tags, const ResultsView& view )
  {
    m_layout = new QHBoxLayout();
    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagView* w = new TagView( tags[i].name );
      w->setScore( tags[i].score );
      m_layout->addWidget( w );
      connect(
        w, SIGNAL( activated( QString ) ),
        &view, SIGNAL( tagRequested( QString ) )
      );
    }
    m_layout->setMargin( 0 );
    m_layout->setAlignment( Qt::AlignLeft );
    this->setLayout( m_layout );
  }
  QHBoxLayout* m_layout;
};

void ResultsView::replaceViewItems( const QModelIndex& index )
{
  // Setting a QWidget accordingly
  if( index.isValid() )
  {
    QWidget* widget = NULL;
    if( m_model->isPreset( index ) )
    {
      const Preset& preset = m_model->getPreset( index );
      PresetView* w = new PresetView( preset.name );
      w->setScore( preset.score );
      widget = w;
    }
    else
    {
      widget = new TagsView( m_model->getTags( index ), *this );
    }
    assert( widget != NULL );
    widget->setMaximumHeight( sizeHintForIndex( index ).height() );
    setIndexWidget( index, widget );
  }

  // Applying recursively to the children
  for( size_t i = 0; i < model()->rowCount( index ); i++ )
    replaceViewItems( model()->index( i, 0, index ) );
}
