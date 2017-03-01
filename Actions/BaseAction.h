// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_ACTION__
#define __UI_BASE_ACTION__

#include <QString>
#include <QAction>
#include <QShortcut>
#include <QKeySequence>

namespace FabricUI {
namespace Actions {

class BaseAction : public QAction
{
  /**
    BaseAction inherates QActions and are registered in the ActionRegistry
    when created. The action unregisteres it-self when it's destroyed. 
    Any actions using shortcuts shall specialize this class.
    
    Usage:
    -C++:
      #include <FabricUI/Actions/BaseAction.h>

      class MyAction : pulic BaseAction
      {
        public:
          MyAction(QObject *parent)
          : BaseAction(
              parent
            , "MyAction" 
            , "Do my action" 
            , Qt::Key_R)
          {
          }
          
          virtual ~MyAction()
          {
          }

        private:
          virtual void onTriggered()
          {
            // Do you action
          }
      };

    -Python:
      from PySide import QtCore
      from FabricEngine.FabricUI import Actions

      class MyAction(Actions.BaseAction):
      
        def __init__(self, parent)
          super(MyAction, self),__init__(
            parent
            , "MyAction" 
            , "Do my action" 
            , QtCore.Qt.Key_R)
          
          def onTriggered(self):
            # Do you action
            pass
  */
  Q_OBJECT

  public:
    BaseAction(
      QObject *parent,
      const QString &name, 
      const QString &text = "", 
      QKeySequence shortcut = QKeySequence(),
      Qt::ShortcutContext context = Qt::ApplicationShortcut,
      bool enable = true,
      const QIcon &icon = QIcon() 
      );

    BaseAction(
      QObject *parent,
      const QString &name, 
      const QString &text = "", 
      const QList<QKeySequence> &shortcuts = QList<QKeySequence>(),
      Qt::ShortcutContext context = Qt::ApplicationShortcut,
      bool enable = true,
      const QIcon &icon = QIcon() 
      );

    virtual ~BaseAction();

    /// Gets the action name.
    QString getName() const;
 
  protected slots:
    /// To override.
    virtual void onTriggered();

  private:
    /// Intializes the BaseAction.
    void init(
      const QString &name, 
      const QString &text = "", 
      Qt::ShortcutContext context = Qt::ApplicationShortcut,
      bool enable = true,
      const QIcon &icon = QIcon());

    /// Action name.
    QString m_name;
};

} // namespace Actions
} // namespace FabricUI

#endif // __UI_DFG_DFGWidget__
