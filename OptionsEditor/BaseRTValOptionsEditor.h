//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_RTVAL_OPTIONS_EDITOR__
#define __UI_BASE_RTVAL_OPTIONS_EDITOR__

#include "BaseOptionsEditor.h"
#include "RTValItem.h"
 
namespace FabricUI {
namespace OptionsEditor {

class BaseRTValOptionsEditor 
  : public BaseOptionsEditor 
{
  /** 
    BaseRTValOptionsEditor is used edit the content of a RTVal in a tree-view widget.
  */
  Q_OBJECT
  
  public:
    /// Initializes the BaseRTValOptionsEditor.
    /// \param client Fabric client.
    /// \param options The options to edit.
    /// \param title Title of the editor.
    /// \param settings Pointor to the settings.
    BaseRTValOptionsEditor(
      FabricCore::Client client,
      const QString &title = QString(),
      void *options=0,
      QSettings *settings=0
      );

    virtual ~BaseRTValOptionsEditor();

    /// Gets the editor title.
    FabricCore::Client getClient();
 
    /// Implementation of BaseOptionsEditor
    virtual ValueEditor::BaseModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor,
      void *options,
      QSettings *settings=0
      );

  protected:
    // Reference to the fabric client.
    FabricCore::Client m_client;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_OPTIONS_EDITOR__
