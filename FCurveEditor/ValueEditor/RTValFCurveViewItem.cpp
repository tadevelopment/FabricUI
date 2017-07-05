//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValFCurveViewItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/AnimXKL/RTValAnimXFCurveModel.h>

#include <FabricUI/ModelItems/DFGModelItemMetadata.h>

using namespace FabricUI::ValueEditor;
using namespace FabricUI::FCurveEditor;

RTValFCurveViewItem::RTValFCurveViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
) : BaseViewItem( name, metadata )
  , m_model( new RTValAnimXFCurveDFGController() )
  , m_editor( new FabricUI::FCurveEditor::FCurveEditor() )
{
  m_editor->setModel( m_model );
  this->onModelValueChanged( value );

  m_editor->setFixedSize( 300, 300 ); // HACK

  connect( m_model, SIGNAL( handleAdded() ), this, SLOT( onViewValueChanged() ) );
  connect( m_model, SIGNAL( handleMoved( size_t ) ), this, SLOT( onViewValueChanged() ) );
  connect( m_editor, SIGNAL( interactionBegin() ), this, SIGNAL( interactionBegin() ) );
  connect( m_editor, SIGNAL( interactionEnd() ), this, SLOT( emitInteractionEnd() ) );

  const char* bindingId = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGBindingIdKey.data() );
  const char* portPath = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGPortPathKey.data() );
  m_model->setValue( bindingId, portPath );
}

RTValFCurveViewItem::~RTValFCurveViewItem()
{
  //delete m_editor; // HACK;TODO : delete and fix crash when closing Canvas
  delete m_model;
}

void RTValFCurveViewItem::onViewValueChanged()
{
  emit this->viewValueChanged( QVariant::fromValue<FabricCore::RTVal>( m_model->value() ) );
}

QWidget* RTValFCurveViewItem::getWidget()
{
  return m_editor;
}

void RTValFCurveViewItem::onModelValueChanged( QVariant const & v )
{
  // TODO : don't clone the value each time, but use incremental commands
  //m_model->setValue( v.value<FabricCore::RTVal>().clone() );
}

BaseViewItem * RTValFCurveViewItem::CreateItem(
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
        return new RTValFCurveViewItem( name, value, metaData );
    }
  }
  return NULL;
}

const int RTValFCurveViewItem::Priority = 3;
