//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_OPTIONS_EDITOR__
#define __UI_RTVAL_OPTIONS_EDITOR__

#include "BaseRTValOptionsEditor.h"
#include "./PathValueResolvers/OptionsPathValueResolver.h"

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
      const QString &title,
      QSettings *settings=0
      );
      
    virtual ~KLOptionsTargetEditor();

    /// Gets the KL registry id.
    QString geteditorID();

    /// \implmentation of BaseRTValOptionsEditor
    virtual BaseRTValModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      BaseRTValOptionsEditor *editor,
      FabricCore::RTVal options,
      QSettings* settings=0
      );

    public slots:  
      /// \implmentation of BaseRTValOptionsEditor
      virtual void resetModel(
        FabricCore::RTVal options=FabricCore::RTVal()
        );
      
      /// \implmentation of BaseRTValOptionsEditor
      virtual void updateModel(
        FabricCore::RTVal options=FabricCore::RTVal()
        );

    private:
      // Reference to the fabric client.
      FabricCore::Client m_client;
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_RTVAL_OPTIONS_EDITOR__
