//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_DICTIONARYVIEWITEM_H
#define FABRICUI_VALUEEDITOR_DICTIONARYVIEWITEM_H

#include "BaseComplexViewItem.h"
#include <FabricCore.h>

class QWidget;

namespace FabricUI {
namespace ValueEditor {

// This implementation supports displaying RTVal dictionaries
// TODO : This class is very similar to ArrayViewItem; merge them,
// the main difference is the way we iterate on elements
// TODO : Currently only works for String Keys
class DictionaryViewItem : public BaseComplexViewItem
{
  Q_OBJECT
  // Cache the core value.  Note - this is a reference, not a copy
  FabricCore::RTVal m_val;

  // A composite widget to represent any/all values in UI
  QWidget *m_widget;

public:

  static BaseViewItem* CreateItem(
    QString const &name, 
    QVariant const &value,
    ItemMetadata* metadata
  );

  static const int Priority;

  DictionaryViewItem(
    QString name,
    const FabricCore::RTVal& value,
    ItemMetadata* metadata
  );
  ~DictionaryViewItem();

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items ) /*override*/;

  virtual void onChildViewValueChanged( int index, QVariant value ) /*override*/;

  virtual QWidget * getWidget() /*override*/;

  virtual void deleteMe() /*override*/ { delete this; };

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_DICTIONARYVIEWITEM_H
