//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "SIntViewItem.h"
#include "QVariantRTVal.h"

#include <QSpinBox>
#include <limits.h>
#include "VEIntSpinBox.h"
#include <QHBoxLayout>

using namespace FabricUI::ValueEditor;

SIntViewItem::SIntViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_spinner = new VEIntSpinBox();
  m_spinner->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );
  m_spinner->setMinimum( INT_MIN );
  m_spinner->setMaximum( INT_MAX );
  m_spinner->setKeyboardTracking( false );
    
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_spinner );
  layout->addStretch();

  m_widget = new QWidget;
  m_widget->setObjectName( "VESIntViewItem" );
  m_widget->setLayout( layout );

  onModelValueChanged( value );

  connect(
    m_spinner, SIGNAL( interactionBegin() ), 
    this, SIGNAL( interactionBegin() )
    );
  connect(
    m_spinner, SIGNAL( valueChanged( int ) ), 
    this, SLOT( OnSpinnerChanged( int ) )
    );
  connect(
    m_spinner, SIGNAL( interactionEnd( bool ) ), 
    this, SIGNAL( interactionEnd( bool ) )
    );

  metadataChanged();
}

SIntViewItem::~SIntViewItem()
{
}

QWidget *SIntViewItem::getWidget()
{
  return m_widget;
}

void SIntViewItem::onModelValueChanged( QVariant const &v )
{
  m_spinner->setValue( getQVariantRTValValue<int>(v) );
}

void SIntViewItem::OnSpinnerChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue<int>( value )
    );
}

void SIntViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<int>( m_spinner->value() )
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* SIntViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if ( RTVariant::isType<int>(value)
    || RTVariant::isType<long long>(value) )
  {
    return new SIntViewItem( name, value, metadata );
  }
  return 0;
}

const int SIntViewItem::Priority = 3;
