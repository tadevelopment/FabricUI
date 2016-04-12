/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_DFG_CANVAS_H__
#define __UI_SCENEHUB_DFG_CANVAS_H__

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <FabricCore.h>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/SceneHub/TreeView/SHTreeItem.h>

namespace FabricUI {
namespace SceneHub {
    
class SHDFGBinding : public QObject{

  Q_OBJECT

  public:
    SHDFGBinding(
      FabricCore::DFGBinding &binding, 
      FabricUI::DFG::DFGController *controller, 
      FabricCore::Client client);

    ~SHDFGBinding() {}
    
    FabricCore::RTVal getSgObject() { return m_dfgCanvasSgObject; }
    
    FabricCore::RTVal getOperator() { return  m_dfgCanvasOperator; }

    bool isSgObjectValid() { return m_dfgCanvasSgObject.isValid(); }

    bool isOperatorValid() { return m_dfgCanvasOperator.isValid(); }

    bool dirtyAllOutputs();


  signals:
    void sceneChanged();
    

  public slots:
    void onArgInserted(unsigned index, FTL::CStrRef name, FTL::CStrRef typeName);

    void onArgRemoved(unsigned index, FTL::CStrRef name);

    void onArgTypeChanged(unsigned index, FTL::CStrRef name, FTL::CStrRef newTypeName);

    void onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *item);


  protected:         
    void connectBindingNotifier();

    FabricCore::DFGBinding m_binding;
    FabricUI::DFG::DFGController *m_controller;
    FabricCore::Client m_client;
    FabricCore::RTVal m_dfgCanvasSgObject;
    FabricCore::RTVal m_dfgCanvasOperator;
};

} // namespace SceneHub
} // namespace FabricUI


#endif // __UI_SCENEHUB_DFG_CANVAS_H__
