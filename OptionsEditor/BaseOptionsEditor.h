//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_OPTIONE_DITOR__
#define __UI_BASE_OPTIONE_DITOR__

#include <string>
#include <QSettings>
#include <QUndoStack>
#include <FabricCore.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>

namespace FabricUI {
namespace Bases {

class OptionsModel;
class OptionsDictModel;

class BaseOptionsEditor : public ValueEditor::VETreeWidget
{
  Q_OBJECT

  public:
    BaseOptionsEditor( 
      FabricCore::Client& client, 
      QUndoStack& undoStack 
      );

    virtual ~BaseOptionsEditor();

    /// 
    void emitValueChanged();

    /// Gets a reference to the undo stack.
    QUndoStack& getUndoStack();
    
  public slots:
    virtual void updateOptions();

  signals:
    /// When the value of one option has changed
    void valueChanged() const;

  protected:
    QSettings m_settings;
    QUndoStack& m_undoStack;
    OptionsDictModel* m_model;
};

} // namespace Bases
} // namespace FabricUI

#endif // __UI_BASE_OPTIONE_DITOR__
