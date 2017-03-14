// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

#include "ItemView.h"

#include <QLayout>

#include <FabricCore.h>
#include <FTL/JSONValue.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <set>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

struct JSONSerializable
{
  std::string toEncodedJSON() const
  {
    FTL::JSONValue* json = this->toJSON();
    std::string dst = json->encode();
    delete json;
    return dst;
  }

protected:
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
class PresetAnd : JSONSerializable
{
  bool m_hasPreset, m_hasOther;
  Preset preset;
  T other;

public:
  PresetAnd() : m_hasPreset( false ), m_hasOther( false ) {}
  void setPreset( const Preset& p )
  {
    m_hasPreset = true;
    this->preset = p;
  }
  void setOther( const T& o )
  {
    m_hasOther = true;
    this->other = o;
  }
  template<typename T2>
  void assign( const T2& o )
  {
    m_hasPreset = o.m_hasPreset;
    m_hasOther = o.m_hasOther;
    this->preset = o.preset;
    this->other = o.other;
  }
  inline bool isUndefined() const { return !m_hasPreset && !m_hasOther; }
  inline bool isPreset() const { return m_hasPreset; }
  inline bool isOther() const { return !m_hasPreset && m_hasOther; }
  inline bool hasOther() const { return m_hasOther; }
  inline Preset& getPreset()
  {
    assert( isPreset() );
    return preset;
  }
  inline const Preset& getPreset() const
  {
    assert( isPreset() );
    return preset;
  }
  inline T& getOther()
  {
    assert( hasOther() );
    return other;
  }
  inline const T& getOther() const
  {
    assert( hasOther() );
    return other;
  }
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    FTL::JSONObject* obj = new FTL::JSONObject();
    obj->insert( "isPreset", new FTL::JSONBoolean( this->isPreset() ) );
    obj->insert( "hasOther", new FTL::JSONBoolean( this->hasOther() ) );
    if( isPreset() )
      obj->insert( "preset", getPreset().toJSON() );
    if( hasOther() )
      obj->insert( "other", getOther().toJSON() );
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
private:
  Tag m_tag;
  bool m_hasTag;
public:
  // Map from the name of to the index of a child
  Indexes tagIndexes;
  TagAndMap() : m_hasTag( false ) {}
  void setTag( const Tag& tag ) { m_hasTag = true; m_tag = tag; }
  inline const Tag& getTag() const { assert( m_hasTag ); return m_tag; }
  inline const bool hasTag() const { return m_hasTag; }
  FTL::JSONValue* toJSON() const FTL_OVERRIDE
  {
    // TODO
    return m_tag.toJSON();
  }
};

// First step : Temporary tree used to gather results by Tag
typedef Node< PresetAnd< TagAndMap > > TmpNode;
TmpNode& AddTag( TmpNode& t, const Tag& tag )
{
  const std::string& key = tag.name;
  if( t.value.isUndefined() )
    t.value.setOther( TagAndMap() );
  Indexes& indexes = t.value.getOther().tagIndexes;
  if( indexes.find( key ) == indexes.end() )
  {
    // If this is a new tag, add it as a child
    indexes.insert( Indexes::value_type( key, t.children.size() ) );
    TagAndMap tam; tam.setTag( tag );
    TmpNode newItem; newItem.value.setOther( tam );
    t.children.push_back( newItem );
  }
  return t.children[indexes[key]];
}

// Second step : Reducing the tree by fusioning consecutive nodes
struct ReducedNode : Node< PresetAnd< Tags > >
{
  ReducedNode( const TmpNode& tmpTI )
  {
    if( !tmpTI.value.isPreset() )
    {
      const TmpNode* n = &tmpTI;
      if( n->value.hasOther() && n->value.getOther().hasTag() )
      {
        this->value.setOther( Tags() );
        this->value.getOther() += n->value.getOther().getTag();
      }
      // If there is a chain of items (Tag or Preset) with
      // only one child, merge them into a single Item
      while( n->children.size() == 1 )
      {
        n = &n->children[0];
        if( n->value.hasOther() && n->value.getOther().hasTag() )
        {
          if( !this->value.isOther() )
            this->value.setOther( Tags() );
          this->value.getOther() += n->value.getOther().getTag();
        }
      }

      if( n->value.isPreset() )
        value.setPreset( n->value.getPreset() );

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
      value.setPreset( tmpTI.value.getPreset() );
  }
};

// Third and last step : this tree is the one
// used by the Qt model
struct ModelNode;
struct ModelValue : PresetAnd<Tags>
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

TmpNode BuildResultTree(
  const std::string& searchResult,
  double& minPresetScore,
  double& maxPresetScore,
  const Query& query
)
{
  const FTL::JSONValue* json = FTL::JSONValue::Decode( searchResult.c_str() );
  const FTL::JSONObject* root = json->cast<FTL::JSONObject>();
  const FTL::JSONArray* resultsJson = root->getArray( "results" );
  
  minPresetScore = std::numeric_limits<double>::max();
  maxPresetScore = std::numeric_limits<double>::min();

  // Set of tags from the Query : no need to display them again
  // since they must be in all the results
  std::set<std::string> queryTags;
  {
    const Query::Tags& queryTagsVec = query.getTags();
    for( size_t i = 0; i < queryTagsVec.size(); i++ )
      queryTags.insert( queryTagsVec[i] );
  }

  TmpNode rootNode;
  for( size_t i = 0; i < resultsJson->size(); i++ )
  {
    const FTL::JSONArray* result = resultsJson->getArray( i );
    const FTL::JSONArray* tags = result->getArray( 2 );
    TmpNode* node = &rootNode;
    for( size_t j = 0; j < tags->size(); j++ )
    {
      const FTL::JSONObject* tagO = tags->getObject( j );
      const std::string tagName = tagO->getString( "tag" );
      if( queryTags.find( tagName ) != queryTags.end() )
        continue; // Ignore tags from the Query
      node = &AddTag( *node, Tag(
        tagName,
        tagO->getFloat64( "weight" )
      ) );
    }
    double presetScore = result->getFloat64( 1 );
    TmpNode newItem; newItem.value.setPreset( Preset(
      result->getString( 0 ),
      presetScore
    ) );
    minPresetScore = std::min( minPresetScore, presetScore );
    maxPresetScore = std::max( maxPresetScore, presetScore );
    node->children.push_back( newItem );
  }

  if ( minPresetScore < maxPresetScore )
    minPresetScore = minPresetScore - 0.05 * (maxPresetScore - minPresetScore);

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

  double minS, maxS;
  Query query;
  TmpNode tmpNode = BuildResultTree( json, minS, maxS, query );
  size_t newCount = Test::LogTree( tmpNode, logFolder + "1_tmpNode.json" );
  originalCount = newCount;

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

  const bool hasTags( const QModelIndex& index ) const
  { return cast( index )->value.hasOther(); }

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
    ReducedNode node( root );
    if( node.value.isUndefined() )
      this->root = node;
    else
    {
      this->root = ModelNode();
      this->root.children.push_back( ModelNode( node ) );
    }
    ComputeParents( this->root );
    this->endResetModel();
  }

  inline bool hasNoResults() const { return rowCount() == 0; }
  inline bool hasSingleResult() const { return hasNoResults() ? false : isPreset( index( 0, 0 ) ); }
  inline bool hasSeveralResults() const { return !hasNoResults() && !hasSingleResult(); }

  inline QModelIndex getFirstPreset() const
  {
    assert( rowCount() > 0 );
    QModelIndex item = this->index( 0, 0 );
    if( isPreset( item ) )
      return item;
    while( rowCount( item ) > 0 )
    {
      item = this->index( 0, 0, item );
      if( isPreset( item ) )
        return item;
    }
    assert( false );
    return QModelIndex();
  }
};

ResultsView::ResultsView()
  : m_model( new Model() )
{
  this->setObjectName( "ResultsView" );
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
  m_model->deleteLater();
}

void ResultsView::validateSelection()
{
  if( !currentIndex().isValid() )
    return;
  else
  if( m_model->isPreset( currentIndex() ) )
    emit presetValidated( this->getSelectedPreset() );
  else
  {
    const Tags& tags = m_model->getTags( currentIndex() );
    std::vector<std::string> names;
    for( size_t i = 0; i < tags.size(); i++ )
      names.push_back( tags[i].name );
    emit tagsRequested( names );
  }
}

void ResultsView::currentChanged( const QModelIndex &current, const QModelIndex &previous )
{
  Parent::currentChanged( current, previous );

  updateHighlight( previous );
  updateHighlight( current );
}

void ResultsView::onSelectionChanged()
{
  if( !currentIndex().isValid() )
    return;
  else
    emit selectingItems();

  if( m_model->isPreset( currentIndex() ) )
    emit presetSelected( this->getSelectedPreset() );
  else
    emit presetDeselected();
}

void ResultsView::setResults( const std::string& searchResult, const Query& query )
{
  // The ViewItems will become obsolete since ModelItems will be destroyed
  m_presetViewItems.clear();
  m_tagsViewItems.clear();

  m_model->setRoot( BuildResultTree( searchResult, this->minPresetScore, this->maxPresetScore, query ) );
  this->expandAll();

  replaceViewItems();

  // Select the first result
  if( !m_model->hasNoResults() )
    this->setCurrentIndex( m_model->getFirstPreset() );
  else
    emit presetDeselected();
  adjustSize();
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

QSize ResultsView::sizeHint() const
{
  QSize s = Parent::sizeHint();
  int height = 0;
  if( model()->rowCount() > 0 )
  {
    // Getting the last item
    QModelIndex index = model()->index( model()->rowCount() - 1, 0 );
    while( model()->rowCount( index ) > 0 )
      index = model()->index( model()->rowCount( index ) - 1, 0, index );
    height = visualRect( index ).bottom() + 1;
    height += contentsMargins().bottom() + contentsMargins().top();
  }
  s.setHeight( std::max( height, 0 ) );
  return s;
}

class ResultsView::TagsView : public QWidget
{
public:
  TagsView( const Tags& tags, const ResultsView& view )
  {
    m_layout = new QHBoxLayout();
    setObjectName( "TagsView" );
    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagView* w = new TagView( tags[i].name );
      m_tagViews.push_back( w );
      w->setScore( tags[i].score );
      m_layout->addWidget( w );
      connect(
        w, SIGNAL( activated( const std::string& ) ),
        &view, SIGNAL( tagRequested( const std::string& ) )
      );
    }
    m_layout->setMargin( 0 );
    m_layout->setAlignment( Qt::AlignLeft );
    this->setLayout( m_layout );
    setHighlighted( false );
  }

  void setHighlighted( bool highlighted )
  {
    SetWidgetHighlight( this, highlighted );
    for( size_t i = 0; i < m_tagViews.size(); i++ )
      m_tagViews[i]->setHighlighted( highlighted );
  }

private:
  std::vector<TagView*> m_tagViews;
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
      std::vector<std::string> tagNames;
      if( m_model->hasTags( index )
        && !m_model->hasSingleResult() ) // Don't show Tags if Single Result
      {
        const Tags& presetTags = m_model->getTags( index );
        for( size_t i = 0; i < presetTags.size(); i++ )
          tagNames.push_back( presetTags[i].name );
      }
      PresetView* w = new PresetView( preset.name, tagNames );
      w->setScore( preset.score, this->minPresetScore, this->maxPresetScore );
      connect(
        w, SIGNAL( requestTag( const std::string& ) ),
        this, SIGNAL( tagRequested( const std::string& ) )
      );
      m_presetViewItems.insert( std::pair<void*,PresetView*>( index.internalPointer(), w ) );
      widget = w;
    }
    else
    {
      TagsView* w = new TagsView( m_model->getTags( index ), *this );
      m_tagsViewItems.insert( std::pair<void*, TagsView*>( index.internalPointer(), w ) );
      widget = w;
    }
    assert( widget != NULL );
    widget->setMaximumHeight( sizeHintForIndex( index ).height() );
    setIndexWidget( index, widget );
  }

  // Applying recursively to the children
  for( int i = 0; i < model()->rowCount( index ); i++ )
    replaceViewItems( model()->index( i, 0, index ) );
}

void ResultsView::updateHighlight( const QModelIndex& index )
{
  bool highlighted = ( index == currentIndex() );
  void* ptr = index.internalPointer();

  PresetViewItems::const_iterator preset = m_presetViewItems.find( ptr );
  if( preset != m_presetViewItems.end() )
    preset->second->setHighlighted( highlighted );

  TagsViewItems::const_iterator tags = m_tagsViewItems.find( ptr );
  if( tags != m_tagsViewItems.end() )
    tags->second->setHighlighted( highlighted );
}

void ResultsView::clearSelection()
{
  this->setCurrentIndex( QModelIndex() );
}
