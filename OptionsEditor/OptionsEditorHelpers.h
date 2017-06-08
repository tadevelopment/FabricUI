//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_HELPERS__
#define __UI_OPTIONS_EDITOR_HELPERS__

#include <QString>
#include <QDockWidget>
#include <FabricCore.h>
#include "BaseRTValOptionsEditor.h"
   
namespace FabricUI {
namespace OptionsEditor {
 
/**
  Set of helpers for Options editor.
*/  

// Qt helpers
/// Creates an options editor widget, adds it to a dock widget.
/// \param editorID ID of the QtWidget (objectName / registryID).
/// \param title Title of the dock widget.
/// \param groupeName Name of the dock widgets group the editor belongs to.
QDockWidget* CreateOptionsEditor( 
  const QString &editorID,
  const QString &title,
  const QString &groupeName = QString()
	);

FabricCore::RTVal GetKLOptionsTargetRegistry();

/// Gets the options of a KL OptionsTarget.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
FabricCore::RTVal GetKLOptionsTargetOptions(
  QString registryID
  );

/// Sets a single option of a KL OptionsTarget.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
void SetKLOptionsTargetSingleOption(
  QString singleOptionPath,
  FabricCore::RTVal singleOption
  ); 

/// Sets a single option of a KL OptionsTarget.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
FabricCore::RTVal GetKLOptionsTargetSingleOption(
  QString singleOptionPath
  ); 

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_HELPERS__
