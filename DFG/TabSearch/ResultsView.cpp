// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

#include <FTL/JSONValue.h>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

// This is a temporary storage
struct TagNode
{
  typedef std::unordered_map<std::string, TagNode> Tags;
  Tags tags;
  typedef std::vector<std::string> Presets;
  Presets presets;

  void reduce();
};

// Gather tags if they are on the same branch and there is only one child per node
void TagNode::reduce()
{
  {
    Tags newChildren;
    for( Tags::iterator cI = tags.begin(); cI != tags.end(); cI++ )
    {
      TagNode& child = cI->second;
      if( child.tags.size() == 1 )
      {
        std::string newKey = cI->first;
        TagNode* newChild = &child;
        while( newChild->presets.size() == 0 && newChild->tags.size() == 1 )
        {
          newKey += " ";
          newKey += newChild->tags.begin()->first;
          newChild = &newChild->tags.begin()->second;
        }
        newChildren.insert( { newKey, *newChild } );
      }
      else
        newChildren.insert( { cI->first, child } );
    }
    this->tags = newChildren;
    for( Tags::iterator cI = tags.begin(); cI != tags.end(); cI++ )
      cI->second.reduce();
  }
};

// Model for the TreeView
class ResultsView::Model : public QAbstractItemModel
{

private:
  struct Node;
  typedef std::vector<Node> Children;
  struct Node
  {
    bool isPreset;
    std::string name;
    Node* parent;
    size_t index;
    Children children;
    Node() {}
    Node( const TagNode& node );
    void computeParentPointers();
  };
  Node root;

  inline const Node* cast( const QModelIndex& index ) const
  { return static_cast<Node*>( index.internalPointer() ); }

public:

  QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const FTL_OVERRIDE
  {
    if( !hasIndex( row, column, parent ) )
      return QModelIndex();
    const Node* parentItem = ( parent.isValid() ? cast( parent ) : &this->root );
    return ( row < parentItem->children.size() ?
      this->createIndex( row, column, (void*) &(parentItem->children[row]) )
      : QModelIndex() );
  }
  QModelIndex parent( const QModelIndex & child ) const FTL_OVERRIDE
  {
    if( !child.isValid() )
      return QModelIndex();
    const Node* parent = cast( child )->parent;
    return ( parent == &root ? QModelIndex() : createIndex( parent->index, 0, (void*)parent ) );
  }
  int rowCount( const QModelIndex & parent = QModelIndex() ) const FTL_OVERRIDE
  {
    //if( parent.column() > 0 ) // TODO : is this required ?
    //  return 0;
    return ( parent.isValid() ? cast( parent )->children.size() : root.children.size() );
  }
  int columnCount( const QModelIndex & parent = QModelIndex() ) const FTL_OVERRIDE { return 1; }
  QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const FTL_OVERRIDE
  {
    return ( index.isValid() && role == Qt::DisplayRole ?
      QString::fromStdString(cast( index )->name) : QVariant() );
  }

  void setRoot( const TagNode& root ) {
    this->beginResetModel();
    this->root = Node( root );
    this->root.computeParentPointers();
    this->endResetModel();
  }
};

ResultsView::Model::Node::Node( const TagNode& node )
{
  this->children.resize( node.tags.size() + node.presets.size() );
  size_t childI = 0;
  for( TagNode::Tags::const_iterator it = node.tags.begin(); it != node.tags.end(); it++ )
  {
    const TagNode& tagN = it->second;
    Node& child = this->children[childI++];
    if( tagN.presets.size() == 1 )
    {
      child.isPreset = true;
      child.name = tagN.presets[0];
    }
    else
    {
      child = Node( tagN );
      child.isPreset = false;
      child.name = it->first;
    }
  }
  for( TagNode::Presets::const_iterator it = node.presets.begin(); it != node.presets.end(); it++ )
  {
    Node& child = this->children[childI++];
    child.isPreset = true;
    child.name = *it;
  }
}

void ResultsView::Model::Node::computeParentPointers()
{
  for( size_t i = 0; i < this->children.size(); i++ )
  {
    Node& child = this->children[i];
    child.parent = this;
    child.index = i;
    child.computeParentPointers();
  }
}

ResultsView::ResultsView()
  : m_model( new Model() )
{
  setModel( m_model );
  this->setHeaderHidden( true );
  connect(
    this->selectionModel(), SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ),
    this, SLOT( selectionChanged() )
  );
  this->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

ResultsView::~ResultsView()
{
  delete m_model;
}

void ResultsView::validateSelection()
{
  //emit presetValidated( this->getSelectedPreset() );
}

void ResultsView::selectionChanged()
{
  //emit presetSelected( this->getSelectedPreset() );
}

void ResultsView::setResults( const std::string& searchResult )
{
  const FTL::JSONValue* json = FTL::JSONValue::Decode( searchResult.c_str() );
  const FTL::JSONObject* root = json->cast<FTL::JSONObject>();
  const FTL::JSONArray* resultsJson = root->getArray( "results" );

  TagNode rootNode;
  for( size_t i = 0; i < resultsJson->size(); i++ )
  {
    const FTL::JSONArray* result = resultsJson->getArray( i );
    const FTL::CStrRef preset = result->getString( 0 );
    const FTL::JSONArray* tags = result->getArray( 2 );
    TagNode* node = &rootNode;
    for( size_t j = 0; j < tags->size(); j++ )
    {
      const FTL::CStrRef tag = tags->getObject( j )->getString( "tag" );
      if( node->tags.find( tag ) == node->tags.end() )
        node->tags.insert( { tag, TagNode() } );
      node = &node->tags[tag];
    }
    node->presets.push_back( preset );
  }
  rootNode.reduce();
  m_model->setRoot( rootNode );
  this->expandAll();

  if( numberResults() > 0 )
    setSelection( 0 );
}

QString ResultsView::getSelectedPreset()
{
  assert( numberResults() > 0 );
  return "";// m_model.data( currentIndex(), Qt::DisplayRole ).value<QString>();
}

void ResultsView::moveSelection( int increment )
{
  if( numberResults() == 0 )
    return;

  int newSelection = currentIndex().row() + increment;
  if( newSelection < 0 )
    newSelection = numberResults() + newSelection;
  else
  if( newSelection >= numberResults() )
    newSelection = newSelection % numberResults();

  setSelection( newSelection );
}

void ResultsView::setSelection( unsigned int index )
{
  //selectionModel()->setCurrentIndex( model()->index( index, 0 ), QItemSelectionModel::SelectCurrent );
}
