// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "QueryEdit.h"

using namespace FabricUI::DFG::TabSearch;

QueryEdit::QueryEdit()
{
  QFont font; font.setPointSize( 16 );
  this->setFont( font );

  connect(
    this, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( onTextChanged( const QString& ) )
  );
}

void QueryEdit::onTextChanged( const QString& text )
{
  m_query.text = text.toStdString();
  emit queryChanged( m_query );
}
