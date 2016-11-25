//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ViewportOptionsEditor.h"

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

using namespace FabricUI::Viewports;

const char namePath_Separator = '/';

// Model for an RTVal option
class ViewportOptionsEditor::ViewportOptionModel : public FabricUI::ValueEditor::BaseModelItem {

  const std::string m_name;
  const std::string m_namePath;
  FabricCore::RTVal m_val; // Current value
  FabricCore::RTVal m_originalValue; // Value before applying the QSettings
  QSettings* m_settings;
  const ViewportOptionsEditor& m_editor;

public:
  ViewportOptionModel(
    const std::string name,
    FabricCore::RTVal value,
    QSettings* settings,
    const std::string namePath,
    const ViewportOptionsEditor& editor
  ) :
    m_name(name),
    m_namePath(namePath + namePath_Separator + name),
    m_val(value),
    m_originalValue(value.clone()),
    m_settings(settings),
    m_editor(editor)
  {
    // Fetching the value from the QSettings
    if (m_settings->contains(m_namePath.data())) {
      QString settingsValue = m_settings->value( m_namePath.data() ).value<QString>();
      m_val.setJSON( settingsValue.toUtf8().data() );
      emit m_editor.valueChanged();
    }
  }

  QVariant getValue() {
    return toVariant(m_val.clone());
  }

  struct OptionUndoCommand : QUndoCommand {

    const QVariant m_previous, m_next;
    ViewportOptionModel& m_model;

    OptionUndoCommand(
      const QString text,
      const QVariant previous,
      const QVariant next,
      ViewportOptionModel& model
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

  void setValue(
    QVariant value
  ) {

    // RTVariant::toRTVal might change the pointer in ioVal,
    // so we copy it to make sure that m_val will always point to the same place
    FabricCore::RTVal m_valCopy = m_val.clone();
    FabricUI::ValueEditor::RTVariant::toRTVal( value, m_valCopy );
    m_val.assign( m_valCopy );

    // Storing the value in the Settings
    m_settings->setValue( m_namePath.data(), QString( m_val.getJSON().getStringCString() ));

    // Updating the UI
    emit m_editor.valueChanged();
    emitModelValueChanged( getValue() );
  }

  void setValue(
    QVariant value,
    bool commit,
    QVariant valueAtInteractionBegin
  ) {

    // might be invalid when changing a Float with the keyboard (as text), for example
    QVariant previousValue = valueAtInteractionBegin.isValid() ? valueAtInteractionBegin : getValue();

    setValue( value );

    if( commit ) {
      m_editor.m_undoStack.push(
        new OptionUndoCommand(
          "Changed the option \"" + QString::fromUtf8( m_namePath.data(), m_namePath.size() ) + "\"",
          previousValue,
          value,
          *this
        )
      );
    }
  }

  bool hasDefault() { return true; }
  void resetToDefault() {

    setValue( toVariant( m_originalValue ), true, getValue() );
  }

  FTL::CStrRef getName() { return m_name; }
};

#include <map>

// Models a dictionary of Options : RTVal[String]
class ViewportOptionsEditor::ViewportOptionsDictModel : public FabricUI::ValueEditor::BaseModelItem {

  const std::string m_name;
  const std::string m_namePath;

  std::map<std::string, BaseModelItem*> m_children;
  std::vector<std::string> m_keys;

public:

  ViewportOptionsDictModel(
    const std::string name,
    FabricCore::RTVal dict,
    QSettings* settings,
    const std::string namePath,
    const ViewportOptionsEditor& editor
  ) : m_name(name),
    m_namePath(namePath + namePath_Separator + name)
  {
    FabricCore::RTVal keys = dict.getDictKeys();
    for (unsigned i = 0; i < keys.getArraySize(); i++) {

      FabricCore::RTVal key = keys.getArrayElementRef(i);
      FabricCore::RTVal value = dict.getDictElement(key);
      if (value.isWrappedRTVal()) { value = value.getUnwrappedRTVal(); }
      BaseModelItem* item = NULL;
      if (value.isDict() && value.dictKeyHasType("String") && value.dictValueHasType("RTVal")) {
        item = new ViewportOptionsDictModel(
          key.getStringCString(),
          value,
          settings,
          m_namePath,
          editor
        );
      }
      else {
        item = new ViewportOptionModel(
          key.getStringCString(),
          value,
          settings,
          m_namePath,
          editor
        );
      }
      m_children[key.getStringCString()] = item;
      m_keys.push_back(key.getStringCString());
    }
  }

  FTL::CStrRef getName() { return m_name; }
  int getNumChildren() { return m_children.size(); }
  BaseModelItem* getChild(FTL::StrRef childName, bool doCreate) { return m_children[childName]; }
  BaseModelItem* getChild(int index, bool doCreate) { return m_children[m_keys[index]]; }
  bool hasDefault() { return true; }
  void resetToDefault() {
    for (std::map<std::string, BaseModelItem*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
      it->second->resetToDefault();
    }
  }
  ~ViewportOptionsDictModel() {
    for (std::map<std::string, BaseModelItem*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
      delete it->second;
    }
  }
};

ViewportOptionsEditor::ViewportOptionsEditor( FabricCore::Client& client, QUndoStack& undoStack )
  : VETreeWidget(), m_settings(), m_undoStack(undoStack), m_model(NULL)
{
   try
  {
    // Create a DrawContext object
    client.loadExtension("InlineDrawing", "", false);
    drawContext = FabricCore::RTVal::Create(client, "DrawContext", 0, 0);
    drawContext = drawContext.callMethod( "DrawContext" , "getInstance", 0, NULL);
    updateOptions();
  }
  catch(FabricCore::Exception e)
  {
    printf("OptionsModel::ViewportOptionsEditor : %s\n", e.getDesc_cstr());
  }
}

// Currently destroying and rebuilding the whole tree :
// we might want to do incremental updates, to keep the state of the items
void ViewportOptionsEditor::updateOptions() {

  if (m_model != NULL) { delete m_model; }

  FabricCore::RTVal dict = drawContext.maybeGetMember("viewportParams");

  m_model = new ViewportOptionsDictModel(
    "Rendering Options",
    dict,
    &m_settings,
    "",
    *this
  );
  this->onSetModelItem(m_model);
}

ViewportOptionsEditor::~ViewportOptionsEditor()
{
  delete m_model;
}
