// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGRegisteredTypeLineEdit.h"
#include <FTL/CStrRef.h>
#include <QMessageBox>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRegisteredTypeLineEdit::DFGRegisteredTypeLineEdit(QWidget * parent, FabricCore::Client & client, QString text)
: DFGAutoCompleteLineEdit(parent, text)
, m_client(client)
{
  onUpdateRegisteredTypeList();
}

void DFGRegisteredTypeLineEdit::onUpdateRegisteredTypeList() {
  FabricCore::Variant registeredTypesVar = FabricCore::GetRegisteredTypes_Variant(m_client);
  for(FabricCore::Variant::DictIter keyIter(registeredTypesVar); !keyIter.isDone(); keyIter.next())
  {
    FTL::CStrRef key = keyIter.getKey()->getStringData();
    if(key.find('<') != key.end())
      continue;
    if(key.find('[') != key.end())
      continue;
    m_registerKLTypeList.append(key.c_str());
  }
  setWords(m_registerKLTypeList);
}

bool DFGRegisteredTypeLineEdit::checkIfTypeExist() {
  return m_registerKLTypeList.contains(text());
}

void DFGRegisteredTypeLineEdit::displayInvalidTypeWarning() {
  QMessageBox::warning(
    this, 
    "Port Edition",
    "The type '" + text() + "' doesn't exist or " + 
    "the extension it depends on has not been loaded yet");
}
