//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_OPTIONS_EDITOR__
#define __UI_BASE_OPTIONS_EDITOR__

#include <QString>
#include <QSettings>
#include "BaseRTValModelItem.h"
#include <FabricUI/ValueEditor/VETreeWidget.h>

namespace FabricUI {
namespace OptionsEditor {

class BaseRTValOptionsEditor : public ValueEditor::VETreeWidget
{
  /**
    BaseRTValOptionsEditor edits the content of a generic options list in a tree-
    view widget. In order to support any kind of data, the options are passed  
    using void pointor. A model composed of nested BaseRTValModelItem (list
    or simple) representing the options hierarchy is created.  
    
    root list item
      - simple item
      - list item
        - simple item
        - simple item   
  */  
  Q_OBJECT

  public:
    /// Constructs a BaseRTValOptionsEditor.
    /// \param title Title of the editor.
    /// \param options The options to edit.
    /// \param settings Pointor to the settings.
    BaseRTValOptionsEditor(
      const QString &title = QString(),
      FabricCore::RTVal options = FabricCore::RTVal(),
      QSettings *settings=0
      );

    virtual ~BaseRTValOptionsEditor();

    /// Constructs recursively the model, to override.
    virtual BaseRTValModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options,
      QSettings* settings=0
      );
 
  public slots:  
    /// Resets the model, to override.
    /// \param options New options for the model.
    virtual void resetModel(
      FabricCore::RTVal options = FabricCore::RTVal()
      );

    /// Updates the model content, to override.
    /// \param options New options for the model.
    virtual void updateModel(
      FabricCore::RTVal options = FabricCore::RTVal()
      );

    /// Updates the editor from the model.
    void modelUpdated();

  signals:
    /// Emitted when the editor changed.
    void updated();

  protected:
    /// Editor's title.
    QString m_title;
    /// Pointor to the settings.
    QSettings *m_settings;
    /// Root model
    BaseRTValModelItem *m_model;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_OPTIONS_EDITOR__
