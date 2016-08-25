//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_WRAPPEDRTVALVIEWITEM_H
#define FABRICUI_VALUEEDITOR_WRAPPEDRTVALVIEWITEM_H

#include "BaseViewItem.h"
#include <FabricCore.h>

namespace FabricUI {
namespace ValueEditor {

// TODO : remove this class, and specialize a QVariant instead ?
/// Wrapping the ViewItem of the Wrapped KL value
class WrappedRTValViewItem : public BaseViewItem
{
  // The Wrapped RTVal
  FabricCore::RTVal m_val;

  // Widget of the unwrapped value
  BaseViewItem *m_widget;

  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
   );
  static const int Priority;

  WrappedRTValViewItem(
    QString name, 
    const FabricCore::RTVal& value,
    ItemMetadata* metadata
   );
  ~WrappedRTValViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual bool hasChildren() const;

  void appendChildViewItems(QList<BaseViewItem*>& items) /*override*/;

  void setWidgetsOnTreeItem(QTreeWidget* treeWidget, QTreeWidgetItem * treeWidgetItem) /*override*/;

  virtual void metadataChanged();

  void deleteMe() { delete this; }

public slots:
  virtual void onModelValueChanged( QVariant const &value );

private slots:
  void onViewValueChanged(QVariant value);
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_WRAPPEDRTVALVIEWITEM_H