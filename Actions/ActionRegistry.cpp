// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ActionRegistry.h"

using namespace FabricUI;
using namespace Actions;

bool ActionRegistry::s_instanceFlag = false;
ActionRegistry* ActionRegistry::s_actionRegistry = 0;

ActionRegistry::ActionRegistry()
{
}

ActionRegistry::~ActionRegistry()
{
  s_instanceFlag = false;
}

ActionRegistry* ActionRegistry::GetActionRegistry()
{
  // Construct the singleton if needed.
  if(!s_instanceFlag)
  {
    s_actionRegistry = new ActionRegistry();
    s_instanceFlag = true;
  }
  return s_actionRegistry;
}

void ActionRegistry::registerAction(
  BaseAction *action)
{
  registerAction(
    action->getName(), 
    action);
}

void ActionRegistry::registerAction(
  const QString &actionName,
  QAction *action)
{
  QObject::connect(
    action,
    SIGNAL(destroyed(QObject *)),
    this,
    SLOT(onUnregisterAction(QObject *))
  );

  m_registeredActions[actionName].append(action);

  emit actionRegistered(actionName, action);
}

void ActionRegistry::onUnregisterAction(
  QObject *obj)
{
  QAction* action = (QAction*)obj;
  if(!action)
      return;

  QString actionName = getActionName(action);
  if(actionName.isEmpty())
    return;

  int count = m_registeredActions[actionName].size();
  for(int i=0; i<count; ++i)
  {
    if (m_registeredActions[actionName][i] == action)
      m_registeredActions[actionName].removeAt(i);
  }

  count = m_registeredActions[actionName].size();
  if(count == 0)
    m_registeredActions.remove(actionName);
 
  emit actionUnregistered(actionName);
}

bool ActionRegistry::isActionRegistered(
  const QString &actionName) const
{
  return m_registeredActions.count(actionName) > 0;
}

int ActionRegistry::getRegistrationCount(
  const QString &actionName) const
{
  if(!isActionRegistered(actionName))
    return 0;

  return m_registeredActions[actionName].size();
}

QAction* ActionRegistry::isShortcutUsed(
  QKeySequence shortcut) const
{
  QMapIterator<QString, QList< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QList< QAction * > registeredAction = ite.value();
    for(int i=0; i<registeredAction.size(); ++i)
    {
      QAction* action = registeredAction[i];
      if(action->shortcut() == shortcut)
        return action;
    }
  }
  return 0;
}

QAction* ActionRegistry::isShortcutUsed(
  const QList<QKeySequence> &shortcuts) const
{
  QMapIterator<QString, QList< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QList< QAction * > registeredAction = ite.value();
    for(int i=0; i<registeredAction.size(); ++i)
    {
      QAction* action = registeredAction[i];
      if(action->shortcuts() == shortcuts)
        return action;
    }
  }
  return 0;
}

void ActionRegistry::setShortcut(
  const QString &actionName,
  QKeySequence shortcut) const
{
  if(!isActionRegistered(actionName))
    return;

  for(int i=0; i<m_registeredActions[actionName].size(); ++i)
    m_registeredActions[actionName][i]->setShortcut(shortcut);
}

void ActionRegistry::setShortcuts(
  const QString &actionName,
  const QList<QKeySequence> &shortcuts) const
{
  if(!isActionRegistered(actionName))
    return;

  for(int i=0; i<m_registeredActions[actionName].size(); ++i)
    m_registeredActions[actionName][i]->setShortcuts(shortcuts);
}
 
QKeySequence ActionRegistry::getShortcut(
  const QString &actionName) const
{
  return isActionRegistered(actionName)
    ? getAction(actionName)->shortcut()
    : QKeySequence();
}

QList<QKeySequence> ActionRegistry::getShortcuts(
  const QString &actionName) const
{
  QList<QKeySequence> dum;
  return isActionRegistered(actionName)
    ? getAction(actionName)->shortcuts()
    : dum;
}

QAction* ActionRegistry::getAction(
  const QString &actionName) const
{
  return isActionRegistered(actionName) 
    ? m_registeredActions[actionName][0]
    : 0;
}

QString ActionRegistry::getActionName(
  QAction *action) const
{
  QMapIterator<QString, QList< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QList< QAction * > registeredAction = ite.value();
    for(int i=0; i<registeredAction.size(); ++i)
    {
      if (registeredAction[i] == action)
        return ite.key();
    }
  }
  return "";
}

QList<QString> ActionRegistry::getActionNameList() const
{
  QList<QString> actionNameList;
  QMapIterator<QString, QList< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    actionNameList.append(ite.key());
  }
  return actionNameList;
}

QString ActionRegistry::getContent() const
{
  QString res = "--> ActionRegistry:\n";
  QMapIterator<QString, QList< QAction * > > ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QList< QAction * > registeredAction = ite.value();
    QAction* action = registeredAction[0];
    QList<QKeySequence> shortcutsList = action->shortcuts();

    res += ite.key();
    res +=+ "[";
    for(int i=0; i<shortcutsList.size(); ++i)
    {
      res += shortcutsList[i].toString();
      res += (i < shortcutsList.size()-1) ? ", " : "";
    }
    res +=+ "]";
    res += '\n';
  }
  return res;
}
