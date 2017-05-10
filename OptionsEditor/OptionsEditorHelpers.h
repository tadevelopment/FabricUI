//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_HELPERS__
#define __UI_OPTIONS_EDITOR_HELPERS__

#include <QList>
#include <QString>
#include <QMainWindow>
#include <QDockWidget>
#include <FabricCore.h>
#include "BaseOptionsEditor.h"
   
namespace FabricUI {
namespace OptionsEditor {
 
/**
  Set of helpers for Options editor.
*/  

// Qt helpers
/// Creates an options editor widget, adds it to a dock widget.
/// \param client Fabric client.
/// \param editorID ID of the QtWidget (objectName / registryID).
/// \param title Title of the dock widget.
/// \param groupeName Name of the dock widgets group the editor belongs to.
QDockWidget* CreateOptionsEditor( 
  FabricCore::Client client,
  const QString &editorID,
  const QString &title,
  const QString &groupeName = QString()
	);

/// Gets the options editor dock.
/// \param editorID ID of the QtWidget (objectName).
QDockWidget* GetOptionsEditorDock(
  const QString &editorID
	);

/// Gets the options editor.
/// \param editorID ID of the QtWidget (objectName).
BaseOptionsEditor* GetOptionsEditor(
  const QString &editorID
	);

// KL generic editor (use for Viewport options editor too)
/// Gets a single KL option.
/// \param client Fabric client.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
FabricCore::RTVal GetKLSingleOption(
  FabricCore::Client client,
  QString singleOptionPath,
  FabricCore::RTVal singleOption);

/// Sets a single KL option.
/// \param client Fabric client.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
/// \param options Single option.
void SetKLSingleOption(
  FabricCore::Client client,
  QString singleOptionPath,
  FabricCore::RTVal singleOption,
  FabricCore::RTVal options);

// KL OptionsTarget editor
/// Gets the options of a KL OptionsTarget.
/// \param client Fabric client.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
FabricCore::RTVal GetKLOptionsTargetOptions(
  FabricCore::Client client,
  QString registryID
  );

/// Sets a single option of a KL OptionsTarget.
/// \param client Fabric client.
/// \param registryID ID of the KL OptionsEditorTarget in registry.
/// \param singleOptionPath Path of the single options if the options hierarchy.
/// \param singleOption Single option.
void SetKLOptionsTargetSingleOption(
  FabricCore::Client client,
  QString registryID,
  QString singleOptionPath,
  FabricCore::RTVal singleOption
  ); 


} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_HELPERS__
