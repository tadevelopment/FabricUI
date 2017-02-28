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
  QMapIterator<QString, QAction*> ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    m_registeredActions.remove(ite.key());
  }
  s_instanceFlag = false;
}

ActionRegistry* ActionRegistry::GetActionRegistry()
{
	if(!s_instanceFlag)
	{
	 	// Set the pointer of the ActionRegistry singleton
	  // equal to this instance of ActionRegistry.
	  s_actionRegistry = new ActionRegistry();
	  s_instanceFlag = true;
	}
 
  return s_actionRegistry;
}

void ActionRegistry::registerAction(
  BaseAction *action)
{
  registerAction(action->getName(), action);
}

void ActionRegistry::registerAction(
  const QString &actionName,
  QAction *action)
{
  m_registeredActions[actionName] = action;
  emit actionRegistered(actionName, action);
}

void ActionRegistry::unregisterAction(
  BaseAction* action)
{
  if(action && isActionRegistered(action->getName()))
  	m_registeredActions.remove(action->getName());
}

bool ActionRegistry::isActionRegistered(
  const QString &actionName) const
{
	return m_registeredActions.count(actionName) > 0;
}

QAction* ActionRegistry::isShortcutUsed(
  QKeySequence shortcut) const
{
 	QMapIterator<QString, QAction*> ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QAction* action = (QAction*)ite.value();
   	if(action->shortcut() == shortcut)
   		return action;
  }
  return 0;
}

QAction* ActionRegistry::isShortcutUsed(
  const QList<QKeySequence> &shortcuts) const
{
  QMapIterator<QString, QAction*> ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QAction* action = (QAction*)ite.value();
    if(action->shortcuts() == shortcuts)
      return action;
  }
  return 0;
}

QAction* ActionRegistry::getAction(
  const QString &actionName) const
{
	return isActionRegistered(actionName) 
		? m_registeredActions[actionName]
		: 0;
}

QList<QString> ActionRegistry::getActionNameList() const
{
  QList<QString> actionNameList;
  QMapIterator<QString, QAction*> ite(m_registeredActions);
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
  QMapIterator<QString, QAction*> ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QAction *action = (QAction *)ite.value();
    QList<QKeySequence> shortcutsList = action->shortcuts();

    res += "["+ ite.key() + "], key:";

    for(int i=0; i<shortcutsList.size(); ++i)
    {
      res += shortcutsList[i].toString() + " ";
    }
    res += '\n';
  }
  return res;
}
