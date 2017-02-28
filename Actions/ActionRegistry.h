// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_ACTION_REGISTRY__
#define __UI_ACTION_REGISTRY__

#include <QMap>
#include <QList>
#include <QObject>
#include "BaseAction.h"

namespace FabricUI {
namespace Actions {

class ActionRegistry : public QObject
{
  /**
    ActionRegistry registers the QAction and BaseAction so we can 
    know which actions and key-shortcuts are used in the application. 
    These informations are not provided by Qt and are needed by the
    command system. 

    The registry sets it-self as a singleton:
    - C++:
      #include <FabricUI/Actions/ActionRegistry.h>
      ActionRegistry *actionRegistry = ActionRegistry::GetActionRegistry();
    
    - Python:
      from FabricEngine.FabricUI import Actions
      actionRegistry = Actions.ActionRegistry.GetActionRegistry()
  
    Usage:
    - Register a QAction: actionRegistry->:registerAction(actionName, new QAction(...));
    
    - Register a BaseAction: actionRegistry->:registerAction(new BaseAction(...));

    - Unregister a BaseAction: actionRegistry->:unregisterAction(new BaseAction(...));

    - Check a action is registered: unregisterAction->isActionRegistered(actionName);
    
    - Check if a shortcut is used: actionRegistry->isShortcutUsed(actionName, QAction *action);

    - Get an action: QAction *action = actionRegistry->getAction(actionName);
  */  
  Q_OBJECT

  public: 
    ActionRegistry();

    ~ActionRegistry();

    /// Gets the registry singleton.
    /// Creates it if needed.
    static ActionRegistry* GetActionRegistry();

    /// Registers a QAction 
    /// under actionName.
    void registerAction(
      const QString &actionName,
      QAction *action
      );

    /// Registers a BaseAction.
    void registerAction(
      BaseAction *action
      );

    /// Unregisters a BaseAction.
    void unregisterAction(
      BaseAction *action
      );
    
    /// Checks if an action is
    /// registered under 'actionName'.
    bool isActionRegistered(
      const QString &actionName
      ) const;

    /// Checks if the shortcut is  
    /// already used by an action.
    QAction* isShortcutUsed(
      QKeySequence shortcut
      ) const;

    /// Checks if the shortcuts is  
    /// already used by an action.
    QAction* isShortcutUsed(
      const QList<QKeySequence> &shortcuts
      ) const;

    /// Gets the action registered under
    /// 'actionName'. Returns null if the
    /// action has not been registered.
    QAction* getAction(
      const QString &actionName
      ) const;

    /// Gets all the registered action.
    QList<QString> getActionNameList() const;

    /// Gets all the registred actions and their
    /// shortcuts as a string, used for debugging.
    QString getContent() const;

  signals:
    /// Emitted when an action 
    /// has been registered.
    void actionRegistered(
      const QString &actionName,
      QAction *action);

  private:
    /// Dictionaries of registered actions.
    QMap< QString, QAction *> m_registeredActions;
    /// Registry singleton.
    static ActionRegistry * s_actionRegistry;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};

} // namespace Actions
} // namespace FabricUI

#endif // __UI_ACTION_REGISTRY__
