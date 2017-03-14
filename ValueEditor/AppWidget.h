//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_KL_WIDGET_VIEWITEM_H
#define FABRICUI_KL_WIDGET_VIEWITEM_H

#include <QCheckBox>
#include <FabricCore.h>
 
namespace FabricUI {
namespace ValueEditor {
 
class BaseViewItem;

class AppWidget : public QObject
{
  /**
    AppWidget creates a kl widget (3D tool) from
    the value-editor to set the data value. It's
    owned by the BaseViewItem that sets it.
  */
  Q_OBJECT

  public:
    AppWidget(
      BaseViewItem *viewItem
      );

    ~AppWidget();

    /// Creates the KL widget and returns
    /// a checbox to set its visibily.
    QCheckBox* createKLWidget(
      FabricCore::RTVal drivenDataType
      );

    /// Updates the widget from
    /// the value-editor.
    void valueChanged(
      FabricCore::RTVal val
      );

  signals:
    /// Emitted when the widget 
    /// is updated.
    void refreshViewPort();

  protected slots:
    /// Set the widget visibility.
    void setVisible(
      bool visibility
      );

  protected:
    /// Get the KL app registry.
    FabricCore::RTVal getAppWidgetRegistry();
    /// Reference to the view item.
    BaseViewItem *m_viewItem;
    /// The KL widget.
    FabricCore::RTVal m_klWidget;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_KL_WIDGET_VIEWITEM_H
