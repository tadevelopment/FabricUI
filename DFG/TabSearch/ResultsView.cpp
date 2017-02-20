// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultsView.h"

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
}

