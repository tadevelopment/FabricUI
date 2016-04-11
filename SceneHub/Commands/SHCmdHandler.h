/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_HANDLER_H__
#define __UI_SCENEHUB_CMD_HANDLER_H__

#include "SHCmd.h"
#include <QtGui/QUndoStack>
#include "SHCmdRegistration.h"

namespace FabricUI {
namespace SceneHub {

class SHCmdHandler : public QObject {
  Q_OBJECT

  protected:
    class WrappedCmd;


  public:
    SHCmdHandler(
      SHGLScene *scene, 
      SHCmdRegistration *cmdRegistration,
      QUndoStack *qUndoStack);

    virtual ~SHCmdHandler() {};

  
  public slots:
    void onSceneUpdated(SHGLScene *scene);

    void onSynchronizeCommands();

    void onAddCommands();


  protected:       
    void addCommand(QString command);
  
    unsigned int m_stackSize;
    QUndoStack *m_qUndoStack;
    SHGLScene *m_shGLScene;
    SHCmdRegistration *m_shCmdRegistration;
};

} // SceneHub
} // FabricUI 

#endif // __UI_SCENEHUB_CMD_HANDLER_H__
