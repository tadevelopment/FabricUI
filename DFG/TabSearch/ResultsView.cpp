// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

#include <iostream>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

void ResultsView::SelectionModel::setCurrentIndex(
  const QModelIndex &index,
  QItemSelectionModel::SelectionFlags command
)
{
  QItemSelectionModel::setCurrentIndex( index, command );
  emit view->presetSelected( view->getSelectedPreset() );
}

ResultsView::ResultsView()
  : m_selectionModel( this )
{
  setModel( &m_model );
  m_selectionModel.setModel( &m_model );
  this->setSelectionModel( &m_selectionModel );
  this->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void ResultsView::setResults( std::vector<std::string> results )
{
  QStringList list;
  for( size_t i = 0; i < results.size(); i++ )
    list.append( QString::fromStdString( results[i] ) );
  m_model.setStringList( list );

  if( numberResults() > 0 )
    setSelection( 0 );
}

QString ResultsView::getSelectedPreset()
{
  assert( numberResults() > 0 );
  return m_model.data( currentIndex(), Qt::DisplayRole ).value<QString>();
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
  selectionModel()->setCurrentIndex( m_model.index( index, 0 ), QItemSelectionModel::SelectCurrent );
}
