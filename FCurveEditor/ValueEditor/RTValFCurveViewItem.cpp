//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValFCurveViewItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/AnimXKL/RTValAnimXFCurveModel.h>

using namespace FabricUI::ValueEditor;
using namespace FabricUI::FCurveEditor;

AnimCurveViewItem::AnimCurveViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
) : BaseViewItem( name, metadata )
  , m_model( new RTValAnimXFCurveModel() )
  , m_editor( new FabricUI::FCurveEditor::FCurveEditor() )
{
  m_editor->setModel( m_model );
  this->onModelValueChanged( value );

  m_editor->setFixedSize( 300, 300 ); // HACK

  connect( m_model, SIGNAL( handleAdded() ), this, SLOT( onViewValueChanged() ) );
  connect( m_model, SIGNAL( handleMoved( size_t ) ), this, SLOT( onViewValueChanged() ) );
}

AnimCurveViewItem::~AnimCurveViewItem()
{
  delete m_editor;
  delete m_model;
}

void AnimCurveViewItem::onViewValueChanged()
{
  emit this->viewValueChanged( QVariant::fromValue<FabricCore::RTVal>( m_model->value() ) );
}

QWidget* AnimCurveViewItem::getWidget()
{
  return m_editor;
}

void AnimCurveViewItem::onModelValueChanged( QVariant const & v )
{
  m_model->setValue( v.value<FabricCore::RTVal>() );
}

BaseViewItem * AnimCurveViewItem::CreateItem(
  QString const& name,
  QVariant const& value,
  ItemMetadata* metaData
)
{
  if( isRTVal( value ) )
  {
    const FabricCore::RTVal& val = value.value<FabricCore::RTVal>();
    if( val.isValid() )
    {
      const char* rtype = val.getTypeNameCStr();
      if( strcmp( rtype, "AnimX::AnimCurve" ) == 0 )
        return new AnimCurveViewItem( name, value, metaData );
    }
  }
  return NULL;
}

const int AnimCurveViewItem::Priority = 3;
