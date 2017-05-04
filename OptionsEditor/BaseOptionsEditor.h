//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_OPTIONS_EDITOR__
#define __UI_BASE_OPTIONS_EDITOR__

#include <QString>
#include <QSettings>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>

namespace FabricUI {
namespace OptionsEditor {

class BaseOptionsEditor : public ValueEditor::VETreeWidget
{
  /**
    BaseOptionsEditor edits the content of a generic options lists
    in a tree-view widget. A model composed of BaseModelItem
    representing the hierarchy of the options is created.     
  */  
  Q_OBJECT

  public:
    /// Constructs a BaseOptionsEditor.
    /// \param title Title of the editor.
    /// \param options The options to edit.
    /// \param settings Pointor to the settings.
    BaseOptionsEditor(
      const QString &title = QString(),
      void *options=0,
      QSettings *settings=0
      );

    virtual ~BaseOptionsEditor();

    /// Constructs recursively the model, to override.
    virtual ValueEditor::BaseModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor,
      void *options,
      QSettings* settings=0
      );

  public slots:  
    /// Resets (clears and sets) the model, to override.
    /// \param options New options for the model.
    virtual void resetModel(
      void *options=0
      );

    /// Updates the model content, to override.
    /// \param options New options for the model.
    virtual void updateModel(
      void *options=0
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
    ValueEditor::BaseModelItem *m_model;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_OPTIONS_EDITOR__
