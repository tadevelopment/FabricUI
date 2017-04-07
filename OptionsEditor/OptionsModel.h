//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_OPTION_MODEL__
#define __UI_BASE_OPTION_MODEL__

#include <string>
#include <QSettings>
#include <FabricCore.h>
#include "BaseOptionsEditor.h"
#include <FabricUI/ValueEditor/BaseModelItem.h>

namespace FabricUI {
namespace Bases {

// Model for an RTVal option
class OptionsModel : public ValueEditor::BaseModelItem {

  public:
    OptionsModel(
      const std::string name,
      FabricCore::RTVal value,
      QSettings* settings,
      const std::string namePath,
      BaseOptionsEditor& editor
    );

    virtual ~OptionsModel();

    /// Implementation of BaseModelItem
    virtual QVariant getValue();

    void setValue(
      QVariant value
    );

    /// Implementation of BaseModelItem
    virtual void setValue(
      QVariant value,
      bool commit,
      QVariant valueAtInteractionBegin
    );

    /// Implementation of BaseModelItem
    virtual bool hasDefault();

    /// Implementation of BaseModelItem
    virtual void resetToDefault();

    /// Implementation of BaseModelItem
    virtual FTL::CStrRef getName();

  protected:
    struct OptionUndoCommand : QUndoCommand {

      const QVariant m_previous, m_next;
      OptionsModel& m_model;

      OptionUndoCommand(
        const QString text,
        const QVariant previous,
        const QVariant next,
        OptionsModel& model
      ) : QUndoCommand( text ),
        m_previous( previous ),
        m_next( next ),
        m_model( model )
      {}

      void undo() {
        m_model.setValue( m_previous );
      }

      void redo() {
        m_model.setValue( m_next );
      }
    };

    const std::string m_name;
    const std::string m_namePath;
    /// Current value
    FabricCore::RTVal m_val; 
    FabricCore::RTVal m_originalValue; 
    /// Value before applying the QSettings
    QSettings* m_settings;
    BaseOptionsEditor& m_editor;
};

} // namespace Bases
} // namespace FabricUI

#endif // __UI_BASE_OPTION_MODEL__
