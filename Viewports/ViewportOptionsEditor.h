//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_VIEWPORT_OPTION_EDITOR_H
#define __UI_VIEWPORT_OPTION_EDITOR_H

#include <QSettings>
#include <QUndoStack>
#include <FabricCore.h>
#include <FabricUI/OptionsEditor/BaseOptionsEditor.h>

namespace FabricUI {
namespace Viewports {

class ViewportOptionsEditor : public OptionsEditor::BaseOptionsEditor
{
  public:
    ViewportOptionsEditor( 
      FabricCore::Client client, 
      QUndoStack* undoStack,
      QSettings *settings
      );

    virtual ~ViewportOptionsEditor();

  public slots:
    virtual void updateOptions();

  private:
    QSettings *m_settings;
    FabricCore::RTVal m_drawContext;
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_VIEWPORT_OPTION_EDITOR_H
