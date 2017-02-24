// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

#include <FTL/JSONValue.h>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

struct Tag
{
  std::string name;
  double score;
  Tag() {}
  Tag( const std::string& name, const double& score ) : name( name ), score( score ) {}
  QString toString() const { return QString::fromStdString(name) + " [" + QString::number( score ) + "]" ; }
};

struct Preset
{
  std::string name;
  double score;
  Preset() {}
  Preset( const std::string& name, const double& score ) : name( name ), score( score ) {}
  QString toString() const { return QString::fromStdString( name ) + " [" + QString::number( score ) + "]"; }
};

struct Tags : public std::vector<Tag>
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
};

// Variant : can be either a Preset, or T
template<typename T>
class PresetOr
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
  inline bool isPreset() const { return type == PRESET; }
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
};

// Generic tree structure, used below
template<typename T>
struct Node
{
  T value;
  std::vector<Node<T>> children;

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
};

typedef std::map<std::string, size_t> Indexes;
struct TagAndMap
{
  Tag tag;
  // Map from the name of to the index of a child
  Indexes tagIndexes;
};

// First step : Temporary tree used to gather results by Tag
typedef Node<PresetOr<TagAndMap>> TmpNode;
TmpNode& AddTag( TmpNode& t, const Tag& tag )
{
  const std::string& key = tag.name;
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
struct ReducedNode : Node<PresetOr<Tags>>
{
  ReducedNode( const TmpNode& tmpTI )
  {
    if( !tmpTI.value.isPreset() )
    {
      const TmpNode* n = &tmpTI;
      this->value.getOther() += n->value.getOther().tag;
      // If there is a chain of items (Tag or Preset) with
      // only one child, merge them into a single Item
      while( n->children.size() == 1 )
      {
        n = &n->children[0];
        if( !n->value.isPreset() )
          this->value.getOther() += n->value.getOther().tag;
      }
      for( const TmpNode& c : n->children )
        this->children.push_back( ReducedNode( c ) );
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

  QString getPresetName( const QModelIndex& index ) const
  { return QString::fromStdString( cast( index )->value.getPreset().name ); }

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
    return ( index.isValid() && role == Qt::DisplayRole ?
      cast( index )->value.toString() : QVariant() );
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
    this, SLOT( selectionChanged() )
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

void ResultsView::selectionChanged()
{
  if( m_model->isPreset( currentIndex() ) )
    emit presetSelected( this->getSelectedPreset() );
  else
    emit presetDeselected();
}

void ResultsView::setResults( const std::string& searchResult )
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

  m_model->setRoot( rootNode );
  this->expandAll();

  if( m_model->rowCount() > 0 )
  {
    const QModelIndex& firstEntry = m_model->index( 0, 0 );
    if( m_model->isPreset( firstEntry ) )
      this->setCurrentIndex( firstEntry );
  }

}

QString ResultsView::getSelectedPreset()
{
  assert( m_model->isPreset( currentIndex() ) );
  return m_model->getPresetName( currentIndex() );
}

void ResultsView::keyPressEvent( QKeyEvent * event )
{
  Parent::keyPressEvent( event );
}
