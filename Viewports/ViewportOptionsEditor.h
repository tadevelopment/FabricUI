//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VIEWPORTOPTIONSEDITOR_H
#define FABRICUI_VALUEEDITOR_VIEWPORTOPTIONSEDITOR_H

#include <FabricCore.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>

#include <QSettings>
#include <QUndoStack>

namespace FabricUI {
namespace Viewports {

  class ViewportOptionsEditor : public FabricUI::ValueEditor::VETreeWidget
  {
    Q_OBJECT

    class ViewportOptionModel;
    class ViewportOptionsDictModel;

    QSettings m_settings;
    QUndoStack& m_undoStack;
    ViewportOptionsDictModel* m_model;
    FabricCore::RTVal drawContext;

  public:
    ViewportOptionsEditor( FabricCore::Client& client, QUndoStack& undoStack );
    virtual ~ViewportOptionsEditor();

  public slots:
    void updateOptions();

  signals:
    /// When the value of one option has changed
    void valueChanged() const;
  };
}
}

#endif // FABRICUI_VALUEEDITOR_VIEWPORTOPTIONSEDITOR_H
