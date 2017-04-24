//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ColorViewItem.h"
#include "QVariantRTVal.h"
#include "ViewItemFactory.h"
#include "ItemMetadata.h"
#include "BaseModelItem.h"
#include "ComboBox.h"

#include <assert.h>
#include <FTL/JSONValue.h>
#include <QVariant>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>

using namespace FabricUI::ValueEditor;

#define IDX_RGB 0
#define IDX_HSV 1
#define META_FORMAT  "displayFormat"

AlphaButton::AlphaButton( QWidget *parent )
  : QPushButton( parent )
{
}

void AlphaButton::paintEvent( QPaintEvent *event )
{
  QPainter painter( this );

  QRect r = rect();

  painter.setPen( Qt::NoPen );

  for ( int row = 0; ; ++row )
  {
    int top = r.top() + 4 * row;
    int bottom = std::min( top + 4, r.bottom() );
    if ( bottom <= top )
      break;

    for ( int col = 0; ; ++col )
    {
      int left = r.left() + 4 * col;
      int right = std::min( left + 4, r.right() );
      if ( right <= left )
        break;
      
      painter.setBrush( ((row + col) % 2 == 0)? Qt::white: Qt::gray );
      painter.drawRect( QRect( left, top, right, bottom ) );
    }
  }

  if ( m_color.isValid() )
  {
    painter.setBrush( m_color );
    painter.drawRect( r );
  }
}

ColorViewItem::ColorViewItem(
  const QVariant& value,
  const QString& name,
  ItemMetadata* metadata
  )
  : BaseComplexViewItem( name, metadata )
  , m_widget( NULL )
  , m_spec( QColor::Rgb )
  , m_specCombo( NULL )
  , m_childMetadata (metadata)
{
  m_alphaButton = new AlphaButton;
  m_alphaButton->setObjectName( "VELeft" );
  connect(
    m_alphaButton, SIGNAL( clicked() ),
    this, SLOT( pickColor() )
    );

  m_specCombo = new ComboBox;
  m_specCombo->setObjectName( "VERight" );
  m_specCombo->addItem( tr( "RGB" ) );
  m_specCombo->addItem( tr( "HSV" ) );
  if ( FTL::StrRef(m_metadata.getString(META_FORMAT)) == FTL_STR("HSV") )
  { m_spec = QColor::Hsv;
    m_specCombo->setCurrentIndex( IDX_HSV ); }
  connect(
    m_specCombo, SIGNAL( currentIndexChanged( const QString& ) ),
    this, SLOT( formatChanged( const QString& ) )
    );

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_alphaButton );
  layout->addWidget( m_specCombo );
  layout->addStretch();

  m_widget = new QWidget;
  m_widget->setObjectName( "VEColorViewItem" );
  m_widget->setLayout( layout );

  m_childMetadata.setString( "uiRange", "(0.0, 1.0)" );

  metadataChanged();

  onModelValueChanged( value );
}

ColorViewItem::~ColorViewItem()
{
}

QWidget *ColorViewItem::getWidget()
{
  return m_widget;
}

bool ColorViewItem::hasAlpha() const
{
  if( isRTVal( m_val ) )
    return true;
  else
  {
    FabricCore::RTVal v;
    RTVariant::toRTVal( m_val, v );
    return !v.hasType( "RGB" );
  }
}

void ColorViewItem::toComponents(
  float &r,
  float &g,
  float &b,
  float &a
  ) const
{
  QColor v = getQVariantRTValValue<QColor>( m_val );
  r = v.redF();
  g = v.greenF();
  b = v.blueF();
  a = v.alphaF();
}

QColor ColorViewItem::toQColor() const
{
  float r, g, b, a;
  toComponents( r, g, b, a );
  return QColorFromComponents( r, g, b, a );
}
 
void ColorViewItem::onModelValueChanged( QVariant const &value )
{
  m_val = value;
  sync();
}

void ColorViewItem::sync()
{
  float r, g, b, a;
  toComponents( r, g, b, a );
  QColor qColor = QColorFromComponents( r, g, b, a );

  switch ( m_spec )
  {
    case QColor::Rgb:
    {
      routeModelValueChanged( 0, QVariant( r ) );
      routeModelValueChanged( 1, QVariant( g ) );
      routeModelValueChanged( 2, QVariant( b ) );
      if ( hasAlpha() )
        routeModelValueChanged( 3, QVariant( a ) );
    }
    break;

    case QColor::Hsv:
    {
      routeModelValueChanged( 0, QVariant( std::max( 0.0, qColor.hueF() ) ) );
      routeModelValueChanged( 1, QVariant( qColor.saturationF() ) );
      routeModelValueChanged( 2, QVariant( qColor.valueF() ) );
      if ( hasAlpha())
        routeModelValueChanged( 3, QVariant( qColor.alphaF() ) );
    }
    break;

    default:
      assert( !"Error: Bad Color Spec in ColorViewItem" );
      break;
  }

  setButtonColor( qColor );
}

void ColorViewItem::onChildViewValueChanged( int index, QVariant value )
{
  QColor newCol;
  switch ( m_spec )
  {
    case QColor::Hsv:
    {
      qreal v[4];
      toQColor().getHsvF( &v[0], &v[1], &v[2], &v[3] );
      v[0] = std::max( 0.0, v[0] );
      v[index] = value.toDouble();
      newCol = QColor::fromHsvF( v[0], v[1], v[2], v[3] );
    }
    break;

    case QColor::Rgb:
    {
      qreal v[4];
      toQColor().getRgbF( &v[0], &v[1], &v[2], &v[3] );
      v[index] = value.toDouble();
      newCol = QColor::fromRgbF( v[0], v[1], v[2], v[3] );
    }
    break;

    default:
      assert( false );
      break;
  }

  emit viewValueChanged( newCol );
}

void ColorViewItem::doAppendChildViewItems( QList<BaseViewItem*>& items )
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();
  BaseViewItem *children[4];
  int childCount = 0;
  switch ( m_spec )
  {
    case QColor::Rgb:
    {
      float r, g, b, a;
      toComponents( r, g, b, a );

      children[childCount++] = factory->createViewItem( "R", QVariant( r ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "G", QVariant( g ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "B", QVariant( b ), &m_childMetadata );
      if ( hasAlpha() )
        children[childCount++] = factory->createViewItem( "A", QVariant( a ), &m_childMetadata );
    }
    break;

    case QColor::Hsv:
    {
      QColor color = toQColor();

      children[childCount++] = factory->createViewItem( "H", QVariant( std::max( 0.0, color.hueF() ) ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "S", QVariant( color.saturationF() ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "V", QVariant( color.valueF() ), &m_childMetadata );
      if ( hasAlpha() )
        children[childCount++] = factory->createViewItem( "A", QVariant( color.alphaF() ), &m_childMetadata );
    }
    break;

    default:
      assert( !"Invalid Color" );
      return;
  }

  for (int i = 0; i < childCount; ++i)
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

void ColorViewItem::setButtonColor( const QColor& color ) 
{
  m_alphaButton->setColor( color );
}

void ColorViewItem::metadataChanged()
{
  FTL::StrRef dispType = m_metadata.getString( META_FORMAT );

  if ( dispType == FTL_STR("HSV") )
  {
    if ( m_spec != QColor::Hsv )
    {
      m_spec = QColor::Hsv;
      m_specCombo->setCurrentIndex( IDX_HSV );
      sync();
    }
  }
  else
  {
    if ( m_spec != QColor::Rgb )
    {
      m_spec = QColor::Rgb;
      m_specCombo->setCurrentIndex( IDX_RGB );
      sync();
    }
  }
}

void ColorViewItem::pickColor()
{
  QColor color = toQColor();
  QColorDialog qcd( color, this->m_widget->parentWidget() );
  qcd.setOption(
    QColorDialog::ShowAlphaChannel,
    hasAlpha()
    );
  
  connect( &qcd, SIGNAL( colorSelected( QColor ) ), 
           this, SLOT( onColorSelected( QColor ) ) );
  connect( &qcd, SIGNAL( currentColorChanged( QColor ) ), 
           this, SLOT( onColorChanged( QColor ) ) );

  emit interactionBegin();
  
  int execResult = qcd.exec();

  // If the user hits cancel, we wish to restore
  // the current state to the previous value
  emit interactionEnd( execResult == QDialog::Accepted );
}

void ColorViewItem::fromQColor( QColor color )
{
  if ( color.isValid() )
  {
    if( isRTVal(m_val) )
      m_val = color;
    else
    {
      FabricCore::RTVal rtval = getQVariantRTValValue<FabricCore::RTVal>( m_val );
      RTVariant::toRTVal( QVariant( color ), rtval );
    }
  }
}

void ColorViewItem::onColorChanged( QColor color )
{
  fromQColor( color );
  emit viewValueChanged( m_val );
}

void ColorViewItem::onColorSelected( QColor color )
{
  fromQColor( color );
  emit viewValueChanged( m_val );
}

void ColorViewItem::formatChanged( const QString& format )
{
  BaseModelItem* modelItem = getModelItem();

  // Note: setting metadata will delete this
  // class
  if ( format == tr("HSV") )
  {
    m_spec = QColor::Hsv;
    if ( modelItem != NULL )
      modelItem->setMetadata( META_FORMAT, "HSV", 0 );
  }
  else
  {
    m_spec = QColor::Rgb;
    if ( modelItem != NULL )
      modelItem->setMetadata( META_FORMAT, "RGB", 0 );
  }

  emit rebuildChildren( this );
}



//////////////////////////////////////////////////////////////////////////
//

BaseViewItem *ColorViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if( RTVariant::isType<QColor>( value ) )
    return new ColorViewItem( value, name, metaData );
  else
    return NULL;
}

const int ColorViewItem::Priority = 3;
