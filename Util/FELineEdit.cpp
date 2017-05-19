//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FELineEdit.h"
#include <QFocusEvent>

using namespace FabricUI::Util;

void FELineEdit::focusInEvent( QFocusEvent *event )
{
  if ( event->reason() != Qt::PopupFocusReason )
    m_last = text();
  QLineEdit::focusInEvent( event );
}

void FELineEdit::focusOutEvent( QFocusEvent *event )
{
  if ( event->reason() != Qt::PopupFocusReason )
    checkText();
  QLineEdit::focusOutEvent( event );
}

void FELineEdit::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter )
  {
    // [FE-6025]
    clearFocus();
    event->accept();
    return;
  }

  if ( event->key() == Qt::Key_Escape )
  {
    // [FE-6007]
    setText(m_last);
    clearFocus();
    event->accept();
    return;
  }

  QLineEdit::keyPressEvent( event );
}
