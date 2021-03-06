//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"
#include "Vec3ViewItem.h"
#include "ViewItemFactory.h"
#include "VEDoubleSpinBox.h"

#include <assert.h>
#include <FabricUI/Util/UIRange.h>
#include <QVariant>
#include <QBoxLayout>
#include <QLineEdit>
#include <QFrame>

using namespace FabricUI::ValueEditor;

Vec3ViewItem::Vec3ViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseComplexViewItem( name, metadata )
  , m_vec3dValue( getQVariantRTValValue<QVector3D>( value ) )
{
  m_widget = new QFrame;
  m_widget->setObjectName( "Vec3Item" );

  m_xSpinBox = new VEDoubleSpinBox;
  m_xSpinBox->setObjectName( "VELeft" );
  m_xSpinBox->setValue( m_vec3dValue.x() );
  m_ySpinBox = new VEDoubleSpinBox;
  m_ySpinBox->setObjectName( "VEMiddle" );
  m_ySpinBox->setValue( m_vec3dValue.y() );
  m_zSpinBox = new VEDoubleSpinBox;
  m_zSpinBox->setObjectName( "VERight" );
  m_zSpinBox->setValue( m_vec3dValue.z() );

  // Connect em up.

  connect(
    m_xSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_xSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_xSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onXSpinBoxValueChanged( double ))
    );

  connect(
    m_ySpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_ySpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_ySpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onYSpinBoxValueChanged( double ))
    );
  
  connect(
    m_zSpinBox, SIGNAL(interactionBegin()),
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_zSpinBox, SIGNAL(interactionEnd(bool)),
    this, SIGNAL(interactionEnd(bool))
    );
  connect(
    m_zSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(onZSpinBoxValueChanged( double ))
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_xSpinBox );
  layout->addWidget( m_ySpinBox );
  layout->addWidget( m_zSpinBox );
  layout->addStretch();

  metadataChanged();
}

Vec3ViewItem::~Vec3ViewItem()
{
}

QWidget *Vec3ViewItem::getWidget()
{
  return m_widget;
}

void Vec3ViewItem::onModelValueChanged( QVariant const &value )
{
  QVector3D newVec3dValue = getQVariantRTValValue<QVector3D>( value );
  if ( newVec3dValue.x() != m_vec3dValue.x() )
  {
    m_xSpinBox->setValue( newVec3dValue.x() );
    routeModelValueChanged( 0, QVariant( newVec3dValue.x() ) );
  }
  if ( newVec3dValue.y() != m_vec3dValue.y() )
  {
    m_ySpinBox->setValue( newVec3dValue.y() );
    routeModelValueChanged( 1, QVariant( newVec3dValue.y() ) );
  }
  if ( newVec3dValue.z() != m_vec3dValue.z() )
  {
    m_zSpinBox->setValue( newVec3dValue.z() );
    routeModelValueChanged( 2, QVariant( newVec3dValue.z() ) );
  }
  m_vec3dValue = newVec3dValue;
}

void Vec3ViewItem::onXSpinBoxValueChanged( double value )
{
  QVector3D vec3d = m_vec3dValue;
  vec3d.setX( value );
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::onYSpinBoxValueChanged( double value )
{
  QVector3D vec3d = m_vec3dValue;
  vec3d.setY( value );
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::onZSpinBoxValueChanged( double value )
{
  QVector3D vec3d = m_vec3dValue;
  vec3d.setZ( value );
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::onChildViewValueChanged(
  int index,
  QVariant value
  )
{
  QVector3D vec3d = m_vec3dValue;
  switch ( index )
  {
    case 0:
      vec3d.setX( value.toDouble() );
      break;
    case 1:
      vec3d.setY( value.toDouble() );
      break;
    case 2:
      vec3d.setZ( value.toDouble() );
      break;
    default:
      assert( false );
      break;
  }
  emit viewValueChanged( QVariant( vec3d ) );
}

void Vec3ViewItem::doAppendChildViewItems(QList<BaseViewItem *>& items)
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();

  BaseViewItem *children[3];
  children[0] = factory->createViewItem( "X", QVariant( m_vec3dValue.x() ), &m_metadata );
  children[1] = factory->createViewItem( "Y", QVariant( m_vec3dValue.y() ), &m_metadata );
  children[2] = factory->createViewItem( "Z", QVariant( m_vec3dValue.z() ), &m_metadata );
  for ( int i = 0; i < 3; ++i )
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

void Vec3ViewItem::metadataChanged()
{
  FTL::StrRef uiRangeString = m_metadata.getString( "uiHardRange" );
  
  double minValue, maxValue;
  if ( FabricUI::DecodeUIRange( uiRangeString, minValue, maxValue ) )
  {
    m_xSpinBox->setRange( minValue, maxValue );
    m_ySpinBox->setRange( minValue, maxValue );
    m_zSpinBox->setRange( minValue, maxValue );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* Vec3ViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if ( RTVariant::isType<QVector3D>( value ) )
    return new Vec3ViewItem( name, value, metadata );
  else
    return 0;
}

const int Vec3ViewItem::Priority = 3;
