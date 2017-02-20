// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "QueryEdit.h"

using namespace FabricUI::DFG::TabSearch;

QueryEdit::QueryEdit()
{
  QFont font; font.setPointSize( 32 );
  this->setFont( font );

  connect(
    this, SIGNAL( textChanged( QString ) ),
    this, SIGNAL( queryChanged( QString ) )
  );
}
