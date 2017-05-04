//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_OPTIONS_EDITOR__
#define __UI_RTVAL_OPTIONS_EDITOR__

#include "BaseRTValOptionsEditor.h"
  
namespace FabricUI {
namespace OptionsEditor {

class KLOptionsEditor 
  : public BaseRTValOptionsEditor
{
  /** 
    KLOptionsEditor specializes BaseRTValOptionsEditor 
    to edit KL OptionsTargets.
  */
  Q_OBJECT
 
  public:
    /// Constructs a RTValDictModelItem .
    /// \param client Fabric core client.
    /// \param registryID ID of the KL OptionsEditorTarget in the registry.
    /// \param title Title of the editor.
    /// \param settings Pointor to the settings.
    KLOptionsEditor(
      FabricCore::Client client,
      const QString &registryID,
      const QString &title = QString(),
      QSettings *settings=0
      );
      
    virtual ~KLOptionsEditor();

    /// Gets the KL registry id.
    QString getRegistryID();

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
      QString m_registryID;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_OPTIONS_EDITOR__
