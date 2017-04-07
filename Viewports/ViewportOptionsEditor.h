//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_VIEWPORT_OPTION_EDITOR_H
#define __UI_VIEWPORT_OPTION_EDITOR_H

#include <FabricCore.h>
#include <FabricUI/OptionsEditor/BaseOptionsEditor.h>

#include <QSettings>
#include <QUndoStack>

namespace FabricUI {
namespace Viewports {

class ViewportOptionsEditor : public Bases::BaseOptionsEditor
{
  Q_OBJECT

  public:
    ViewportOptionsEditor( 
      FabricCore::Client& client, 
      QUndoStack& undoStack 
      );

  public slots:
    virtual void updateOptions();

  private:
    FabricCore::RTVal m_drawContext;
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_VIEWPORT_OPTION_EDITOR_H
