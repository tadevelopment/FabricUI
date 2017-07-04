//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_KL_TOOL_VIEWITEM_H
#define FABRICUI_KL_TOOL_VIEWITEM_H

#include <QCheckBox>
#include <FabricCore.h>
 
namespace FabricUI {
namespace Tools {
 
class AppTool : public QObject
{
  /**
    AppTool creates a kl tool from
    the value-editor to set the data value. It's
    owned by the BaseViewItem that sets it.
  */
  Q_OBJECT

  public:
    AppTool(
      //BaseViewItem *viewItem
      );

    ~AppTool();

    /// Creates the KL tool and returns
    /// a checbox to set its visibily.
    QCheckBox* createKLTool(
      FabricCore::RTVal drivenDataType
      );

    /// Updates the tool from
    /// the value-editor.
    void valueChanged(
      FabricCore::RTVal val
      );

  signals:
    /// Emitted when the tool 
    /// is updated.
    void refreshViewPort();

  public slots:
    void onToggleManipulation(
      bool toggled
      );

  protected slots:
    /// Set the tool visibility.
    void setVisible(
      bool visibility
      );

  protected:
    /// Get the KL app registry.
    FabricCore::RTVal getAppToolRegistry();
    /// Reference to the view item.
    //BaseViewItem *m_viewItem;
    /// The KL tool.
    FabricCore::RTVal m_klTool;
    QCheckBox* m_checkbox;
};

} // namespace Tools 
} // namespace FabricUI 

#endif // FABRICUI_KL_TOOL_VIEWITEM_H
