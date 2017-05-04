//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_HELPERS__
#define __UI_OPTIONS_EDITOR_HELPERS__

#include <Qlist>
#include <QString>
#include <QMainWindow>
#include <QDockWidget>
#include <FabricCore.h>
#include "BaseOptionsEditor.h"
   
namespace FabricUI {
namespace OptionsEditor {
 
/**
  BaseOptionsEditorCommand defines a base command for interacting 
  with. commands should specialized it.
*/  

/// Gets the main widget (Qt widget containing the options editor dock).
QMainWindow* GetMainWindow();

/// Creates a options editor widget, and add it to a dock widget.
/// \param client Fabric core client.
/// \param title Title of the dock widget.
/// \param name Name of the  of the KL OptionsEditorTarget in the registry.
QDockWidget* CreateOptionsEditor( 
  FabricCore::Client client,
  const QString &registryID,
  const QString &title,
  const QString &groupeName = QString()
	);

/// Get the options editor dock.
/// \param name ID of the KL OptionsEditorTarget in registry.
QDockWidget* GetOptionsEditorDock(
  const QString &registryID
	);

QDockWidget* GetOptionsEditorDockByTitle(
  const QString &title
  );

// Get the options editor dock.
QList<QDockWidget*> GetOptionsEditorDocks();

/// Get the options editor.
/// \param name ID of the KL OptionsEditorTarget in registry.
BaseOptionsEditor* GetOptionsEditor(
  const QString &registryID
	);
 
FabricCore::RTVal GetKLOptionsEditorTargetRegistry(
  FabricCore::Client client
  );

FabricCore::RTVal GetKLOptionsTargetOptions(
  FabricCore::Client client,
  QString registryID
  );

void SetKLOptionsTargetSingleOption(
  FabricCore::Client client,
  QString registryID,
  QString optionPath,
  FabricCore::RTVal rtVal
  ); 

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_HELPERS__
