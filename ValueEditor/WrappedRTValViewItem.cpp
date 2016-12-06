//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "WrappedRTValViewItem.h"
#include "ViewItemFactory.h"
#include "QVariantRTVal.h"

using namespace FabricUI::ValueEditor;

WrappedRTValViewItem::WrappedRTValViewItem(
  QString name,
  const FabricCore::RTVal& value,
  ItemMetadata* metadata
) : BaseViewItem( name, metadata )
  , m_val(value)
{
  if(!m_val.isValid())
    return;

  try
  {
    // Creating a view for the unwrapped RTVal
    ViewItemFactory* factory = ViewItemFactory::GetInstance();
    FabricCore::RTVal v = m_val.getUnwrappedRTVal();
    m_widget = 
      factory->createViewItem(
        name,
        toVariant( v ),
        &m_metadata
      );

    // Connecting each signal to the child view
    if( m_widget != NULL ) {
      connect(
        m_widget, SIGNAL( interactionBegin() ),
        this, SIGNAL( interactionBegin() )
      );
      connect(
        m_widget, SIGNAL( viewValueChanged( QVariant ) ),
        this, SLOT( onViewValueChanged( QVariant )) // We intercept the QVariant here, to wrap it
      );
      connect(
        m_widget, SIGNAL( interactionEnd( bool ) ),
        this, SIGNAL( interactionEnd( bool ) )
      );
      connect(
        this, SIGNAL( rebuildChildren(FabricUI::ValueEditor::BaseViewItem*) ),
        m_widget, SIGNAL( rebuildChildren(FabricUI::ValueEditor::BaseViewItem*) )
      );
    }
  } catch( FabricCore::Exception e ) {
    printf("WrappedRTValViewItem::WrappedRTValViewItem : %s\n", e.getDesc_cstr() );
  }
}

WrappedRTValViewItem::~WrappedRTValViewItem()
{
}

QWidget* WrappedRTValViewItem::getWidget()
{
  return m_widget->getWidget();
}

bool WrappedRTValViewItem::hasChildren() const
{
  return m_widget->hasChildren();
}

void WrappedRTValViewItem::appendChildViewItems(QList<BaseViewItem*>& items)
{
  m_widget->appendChildViewItems(items);
}

void WrappedRTValViewItem::metadataChanged()
{
  m_widget->metadataChanged();
}

void WrappedRTValViewItem::onModelValueChanged( QVariant const &value )
{
  m_widget->onModelValueChanged( value );
}

void WrappedRTValViewItem::setWidgetsOnTreeItem(QTreeWidget* treeWidget, QTreeWidgetItem * treeWidgetItem)
{
  m_widget->setWidgetsOnTreeItem(treeWidget, treeWidgetItem);
}

void WrappedRTValViewItem::onViewValueChanged(QVariant value) {

  try {

    // Unwrapping the RTVal
    FabricCore::RTVal unwrapped = m_val.getUnwrappedRTVal();
    // Assigning the value to it
    RTVariant::toRTVal(value, unwrapped);
    emit viewValueChanged(toVariant(m_val));

  } catch (FabricCore::Exception e) {
    printf("WrappedRTValViewItem::onViewValueChanged : %s\n", e.getDesc_cstr());
  }
}

BaseViewItem * WrappedRTValViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
) {

  if (value.type() != QVariant::UserType)
    return NULL;
  if (value.userType() != qMetaTypeId<FabricCore::RTVal>())
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if( rtVal.isValid() && rtVal.isWrappedRTVal() )
  {
    WrappedRTValViewItem* pViewItem = new WrappedRTValViewItem( QString(name), rtVal, metaData );
    return pViewItem;
  }
  return NULL;
}

const int WrappedRTValViewItem::Priority = 2;
