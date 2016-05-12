// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef _SHDFGCOMBINEDWIDGET_H_
#define _SHDFGCOMBINEDWIDGET_H_
 
#include <FabricUI/SceneHub/SHStates.h>
#include <FabricUI/SceneHub/SHGLScene.h>
#include <FabricUI/DFG/DFGCombinedWidget.h>
#include <FabricUI/SceneHub/DFG/SHDFGBinding.h>
#include <FabricUI/SceneHub/TreeView/SHBaseTreeViewsManager.h>

using namespace FabricServices;
 
namespace FabricUI {
namespace DFG {
    
class SHDFGCombinedWidget : public DFGCombinedWidget {
    
  Q_OBJECT
  
  public:
    SHDFGCombinedWidget(QWidget * parent);

    virtual ~SHDFGCombinedWidget() {};


  protected slots:
    void onInspectChanged();

    void onCanvasSidePanelInspectRequested();

    void onModelValueChanged(FabricUI::ValueEditor::BaseModelItem *item, QVariant const &newValue);

    void onActiveSceneChanged(FabricUI::SceneHub::SHGLScene *scene);
  

  protected:
    virtual void refreshScene() = 0;  

    /// Implementation of DFG::DFGCombinedWidget.
    virtual void initDFG();

    /// Implementation of DFG::DFGCombinedWidget.
    virtual void initTreeView();

    /// Implementation of DFG::DFGCombinedWidget.
    virtual void initValueEditor();
    
    /// Implementation of DFG::DFGCombinedWidget.
    virtual void initDocks();
  

    SceneHub::SHStates *m_shStates;
    SceneHub::SHDFGBinding *m_shDFGBinding;
    SceneHub::SHBaseTreeViewsManager *m_shTreeViewsManager;
};

} // namespace DFG
} // namespace FabricUI


#endif 
