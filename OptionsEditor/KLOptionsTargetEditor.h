//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_OPTIONS_EDITOR__
#define __UI_RTVAL_OPTIONS_EDITOR__

#include "BaseRTValOptionsEditor.h"
  
namespace FabricUI {
namespace OptionsEditor {

class KLOptionsTargetEditor 
  : public BaseRTValOptionsEditor
{
  /** 
    KLOptionsTargetEditor specializes BaseRTValOptionsEditor 
    to edit KL OptionsTargets, see Exts/FabricInterfaces/OptionsEditor.
  */
  Q_OBJECT
 
  public:
    /// Constructs a RTValDictModelItem .
    /// \param client Fabric core client.
    /// \param editorID ID of the KL OptionsEditorTarget in the registry.
    /// \param title Title of the editor.
    /// \param settings Pointor to the settings.
    KLOptionsTargetEditor(
      FabricCore::Client client,
      const QString &editorID,
      const QString &title = QString(),
      QSettings *settings=0
      );
      
    virtual ~KLOptionsTargetEditor();

    /// Gets the KL registry id.
    QString geteditorID();

    /// \implmentation of BaseOptionsEditor
    virtual ValueEditor::BaseModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      BaseOptionsEditor *editor,
      void *options,
      QSettings* settings=0
      );

    public slots:  
      /// \implmentation of BaseOptionsEditor
      virtual void resetModel(
        void *options=0
        );
      
      /// \implmentation of BaseOptionsEditor
      virtual void updateModel(
        void *options=0
        );

    private:
      /// ID the the KL OptionsTarget in the registry.
      QString m_editorID;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_OPTIONS_EDITOR__
