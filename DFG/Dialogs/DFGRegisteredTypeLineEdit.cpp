// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

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
  m_registerKLTypeList.clear();
  for(FabricCore::Variant::DictIter keyIter(registeredTypesVar); !keyIter.isDone(); keyIter.next())
  {
    FTL::CStrRef key = keyIter.getKey()->getStringData();
    if (   key.find('<') != key.end()
        || key.find('[') != key.end() )
      continue;
    m_registerKLTypeList.append( QString(key.c_str()) );
    m_registerKLTypeList.append( QString(key.c_str()) + "[]");
  }
  // Use for auto-completion
  setWords(m_registerKLTypeList);
}

bool DFGRegisteredTypeLineEdit::checkIfTypeExist() {
  bool isValid = false;
  // Throw an exception if the type is malformed (e.g RTVal[string})
  try
  {
    isValid = ( text().startsWith('$') && text().endsWith('$') && text().size() > 2 ) ||
      m_client.isValidType(text().toUtf8().data());
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  } 
  return isValid;
}

void DFGRegisteredTypeLineEdit::displayInvalidTypeWarning() {
  QMessageBox::warning(
    this, 
    "Port Edition",
    "The type '" + text() + "' doesn't exist or " + 
    "the extension it depends on has not been loaded yet");
}
