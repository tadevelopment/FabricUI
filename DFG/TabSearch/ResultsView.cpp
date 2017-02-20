// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

#include <iostream>

using namespace FabricUI::DFG::TabSearch;

ResultsView::ResultsView()
{
  setModel( &m_model );
  this->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void ResultsView::setResults( std::vector<std::string> results )
{
  QStringList list;
  for( int i = 0; i < results.size(); i++ )
    list.append( QString::fromStdString( results[i] ) );
  m_model.setStringList( list );

  if( list.size() > 0 )
    setSelection( 0 );
}

void ResultsView::moveSelection( int increment )
{
  if( m_model.rowCount() == 0 )
    return;

  int newSelection = currentIndex().row() + increment;
  if( newSelection < 0 )
    newSelection = m_model.rowCount() + newSelection;
  else
  if( newSelection >= m_model.rowCount() )
    newSelection = newSelection % m_model.rowCount();

  setSelection( newSelection );
}

void ResultsView::setSelection( unsigned int index )
{
  selectionModel()->setCurrentIndex( m_model.index( index, 0 ), QItemSelectionModel::SelectCurrent );
}
