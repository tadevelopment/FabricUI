// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGGetStringDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

/// Constructor
DFGGetStringDialog::DFGGetStringDialog(QWidget * parent, QString title, QString text, const DFGConfig & dfgConfig, bool setAlphaNum)
: DFGBaseDialog(parent, true, dfgConfig)
{
  this->setWindowTitle(title);
  m_lineEdit = new QLineEdit(text);
  m_lineEdit->selectAll();
  m_lineEdit->setMinimumWidth(200);
  addInput(m_lineEdit, "Node Name");

  // [Julien] Allows only alpha-numeric text only
  // We do this because the nodes's name must be alpha-numerical only
  // and should not contain "-, +, ?,"
  if(setAlphaNum) alphaNumicStringOnly();

  // Adjust the size of the dialog and flag it so it can't be maximized/resized
  this->adjustSize();
  this->window()->layout()->setSizeConstraint( QLayout::SetFixedSize );
}

/// Destructor
DFGGetStringDialog::~DFGGetStringDialog()
{
}

/// Allows only alpha-numeric text only 
void DFGGetStringDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("[a-zA-Z][_a-zA-Z0-9]*"));
}

/// Filters the QLineEdit text with the regexFilter
void DFGGetStringDialog::setRegexFilter(QString regexFilter) {
  if(m_lineEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_lineEdit->setValidator(validator);
  }
}

/// Gets the text
QString DFGGetStringDialog::text() const
{
  return m_lineEdit->text();
}

