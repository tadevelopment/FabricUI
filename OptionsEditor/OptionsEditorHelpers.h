//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_HELPERS__
#define __UI_OPTIONS_EDITOR_HELPERS__

#include <QList>
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
/// \param context Fabric context.
/// \param editorID ID of the QtWidget (objectName / registryID).
/// \param title Title of the dock widget.
/// \param groupeName Name of the dock widgets group the editor belongs to.
QDockWidget* CreateOptionsEditor( 
  FabricCore::Client client,
  const QString &editorID,
  const QString &title,
  const QString &groupeName = QString(),
  QSettings *settings = 0
	);

/// Gets the options editor dock.
/// \param editorID ID of the QtWidget (objectName).
QDockWidget* GetOptionsEditorDock(
  const QString &editorID
	);

/// Gets the options editor.
/// \param editorID ID of the QtWidget (objectName).
BaseRTValOptionsEditor* GetOptionsEditor(
  const QString &editorID
	);

FabricCore::RTVal GetKLOptionsTargetRegistry(
  FabricCore::Context context);

/// Gets the options of a KL OptionsTarget.
/// \param context Fabric context.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
FabricCore::RTVal GetKLOptionsTargetOptions(
  FabricCore::Context context,
  QString registryID
  );

/// Sets a single option of a KL OptionsTarget.
/// \param context Fabric context.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
void SetKLOptionsTargetSingleOption(
  QString singleOptionPath,
  FabricCore::RTVal singleOption
  ); 

/// Sets a single option of a KL OptionsTarget.
/// \param context Fabric context.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
FabricCore::RTVal GetKLOptionsTargetSingleOption(
  QString singleOptionPath,
  FabricCore::Context context
  ); 

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_HELPERS__
