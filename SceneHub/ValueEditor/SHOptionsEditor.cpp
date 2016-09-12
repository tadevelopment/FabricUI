//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "SHOptionsEditor.h"

using namespace FabricUI::SceneHub;

QVariant SHOptionsModel::getValue()
{
  try
  {
    return toVariant(this->shStates.getOptionsRef());
  }
  catch(FabricCore::Exception e)
  {
    printf("SHOptionsModel::getValue : %s\n", e.getDesc_cstr());
  }
  return QVariant();
}

void SHOptionsModel::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin
)
{
  // TODO : transmit the commit boolean
  this->shStates.updateFromOptions();
}

SHOptionsEditor::SHOptionsEditor( SHStates& shStates )
  : FabricUI::ValueEditor::VETreeWidget()
{
  this->onSetModelItem( new SHOptionsModel( shStates ) );
}

SHOptionsEditor::~SHOptionsEditor()
{

}
