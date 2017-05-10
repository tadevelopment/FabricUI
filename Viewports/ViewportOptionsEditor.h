//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_VIEWPORT_OPTION_EDITOR_H
#define __UI_VIEWPORT_OPTION_EDITOR_H

#include <FabricUI/OptionsEditor/RTValModelItem.h>
#include <FabricUI/OptionsEditor/BaseRTValOptionsEditor.h>
#include <FabricUI/OptionsEditor/Commands/BaseSetRTValModelItemCommand.h>

namespace FabricUI {
namespace Viewports {

class SetViewportModelItemCommand 
  : public OptionsEditor::BaseSetRTValModelItemCommand
{
  public:
    SetViewportModelItemCommand();

    virtual ~SetViewportModelItemCommand();

    /// Implementation of Command.
    virtual QString getHelp();

  protected:
    /// Implementation of BaseSetRTValModelItemCommand.
    virtual void setSingleOption(
      const QString &editorID,
      const QString &optionsPath,
      FabricCore::RTVal singleOption
      );

    /// Implementation of BaseSetRTValModelItemCommand.
    virtual FabricCore::RTVal getOptions(
      const QString &editorID
      );
};

class ViewportModelItem 
  : public OptionsEditor::RTValModelItem
{  
  Q_OBJECT
  
  public:
    ViewportModelItem(
      const std::string &name,
      const std::string &path,
      OptionsEditor::BaseOptionsEditor *editor,
      void *options,
      QSettings *settings);
    
    virtual ~ViewportModelItem();

    /// Implementation of ValueEditor::BaseModelItem
    virtual void setValue(
      QVariant value,
      bool commit = false,
      QVariant valueAtInteractionBegin = QVariant());
};

class ViewportOptionsEditor 
  : public OptionsEditor::BaseRTValOptionsEditor
{
  Q_OBJECT
 
  public:
    ViewportOptionsEditor( 
      FabricCore::Client client, 
      QSettings *settings
      );

    virtual ~ViewportOptionsEditor();
    
    /// Implementation of BaseOptionsEditor.
    virtual ValueEditor::BaseModelItem* constructModel(
      const std::string &name,
      const std::string &path,
      OptionsEditor::BaseOptionsEditor *editor,
      void *options,
      QSettings* settings=0
      );

    FabricCore::RTVal getViewportParams();

  public slots:  
    /// Implementation of BaseOptionsEditor.
    virtual void resetModel(
      void *options=0
      );

    /// Implementation of BaseOptionsEditor.
    virtual void updateModel(
      void *options=0
      );
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_VIEWPORT_OPTION_EDITOR_H
