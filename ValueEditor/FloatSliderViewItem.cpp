//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DoubleSlider.h"
#include "ViewConstants.h"
#include "FloatSliderViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <FabricUI/Util/UIRange.h>
#include <float.h>
#include <FTL/JSONValue.h>
#include <FTL/AutoSet.h>
#include <QVariant>
#include <QHBoxLayout>


using namespace FabricUI::ValueEditor;

FloatSliderViewItem::FloatSliderViewItem(
  QString const &name,
  QVariant const &variant,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_isSettingValue(false)
{
  m_lineEdit = new VELineEdit;
  m_lineEdit->setObjectName( "VELeft" );
  m_lineEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

  m_slider = new DoubleSlider;
  m_slider->setObjectName( "VERight" );
  m_slider->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

  metadataChanged();

  double value = getQVariantRTValValue<double>(variant);

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, m_slider->min(), value);

  m_lineEdit->setValidatorDouble();
  m_lineEdit->setText( QString::number( value ) );
  m_slider->setDoubleValue( value );

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_lineEdit );
  layout->addWidget( m_slider );
  
  layout->setStretchFactor(m_lineEdit, 1);
  layout->setStretchFactor(m_slider, 5);  

  m_widget = new QWidget;
  m_widget->setObjectName( "VEFloatSliderViewItem" );
  m_widget->setLayout( layout );

  connect(
    m_lineEdit, SIGNAL( textModified( QString ) ),
    this, SLOT( onLineEditTextModified( QString ) )
    );

  connect(
    m_slider, SIGNAL( sliderPressed() ),
    this, SLOT( onSliderPressed() )
    );
  connect(
    m_slider, SIGNAL( doubleValueChanged( double ) ),
    this, SLOT( onDoubleValueChanged( double ) )
    );
  connect(
    m_slider, SIGNAL( sliderReleased() ),
    this, SLOT( onSliderReleased() )
    );
}

FloatSliderViewItem::~FloatSliderViewItem()
{
}

QWidget *FloatSliderViewItem::getWidget()
{
  return m_widget;
}

void FloatSliderViewItem::onModelValueChanged( QVariant const &v )
{
  FTL::AutoSet<bool> settingValue(m_isSettingValue, true);

  double value = getQVariantRTValValue<double>(v);

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, m_slider->min(), value);

  m_slider->setDoubleValue( value, false );  
  m_lineEdit->setText( QString::number( value ) );
}

void FloatSliderViewItem::metadataChanged()
{
  // metadata should be like this:
  // uiRange: "(0, 200)"
  // uiHardRange: "(0, 500)"

  if ( !FabricUI::DecodeUIRange(
    m_metadata.getString( "uiRange" ),
    m_softMinimum,
    m_softMaximum
    ) )
  {
    m_softMinimum = 0.0;
    m_softMaximum = 1.0;
  }

  if ( !FabricUI::DecodeUIRange(
    m_metadata.getString( "uiHardRange" ),
    m_hardMinimum,
    m_hardMaximum
    ) )
  {
    m_hardMinimum = -DBL_MAX;
    m_hardMaximum = +DBL_MAX;
  }

  m_softMinimum = std::max( m_softMinimum, m_hardMinimum );
  m_softMaximum = std::min( m_softMaximum, m_hardMaximum );

  m_slider->setResolution( FLOAT_SLIDER_DECIMALS, m_softMinimum, m_softMaximum );
}

void FloatSliderViewItem::onLineEditTextModified( QString text )
{
  double value = std::max(
    m_hardMinimum,
    std::min(
      m_hardMaximum,
      Util::tolerantStringToDouble(text)
      )
    );

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(FLOAT_SLIDER_DECIMALS, m_slider->min(), value);

  if ( value != m_slider->doubleValue() )
    m_slider->setDoubleValue( value, true /* emitSignal */ );
  else
    m_lineEdit->setText( QString::number( value ) );
}

void FloatSliderViewItem::onSliderPressed()
{
  emit interactionBegin();
}

void FloatSliderViewItem::onDoubleValueChanged( double value )
{
  if(m_isSettingValue)
    return;
  m_lineEdit->setText( QString::number( value ) );
  emit viewValueChanged( QVariant::fromValue<double>( value ) );
}

void FloatSliderViewItem::onSliderReleased()
{
  emit interactionEnd( true );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* FloatSliderViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (metaData == NULL)
    return NULL;

  if (RTVariant::isType<double>(value) || RTVariant::isType<float>(value))
  {
    // We can only create the UI if we have a defined Min & Max
    if (metaData->has( "uiRange" ) || metaData->has( "uiHardRange" ))
    {
      FloatSliderViewItem* item = new FloatSliderViewItem( name, value, metaData );
      return item;
    }
  }
  return NULL;
}

const int FloatSliderViewItem::Priority = 5;
