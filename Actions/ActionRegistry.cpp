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
  // Check if the actions hasn't been registered already.
  // Usefull to debug
  QAction *temp = getAction(actionName);

  if( temp && temp->shortcuts() == action->shortcuts() )
  {
    QString sequence;
    QList<QKeySequence> shortcutsList = action->shortcuts();
    for(int i=0; i<shortcutsList.size(); ++i)
    {
      sequence += shortcutsList[i].toString();
      sequence += (i < shortcutsList.size()-1) ? ", " : "";
    }

    printf(
      "ActionRegistry::registerAction, action '%s[%s]' already registered\n", 
      actionName.toUtf8().constData(),
      sequence.toUtf8().constData()
    );

    return;
  }

  m_registeredActions[actionName] = action;
  emit actionRegistered(actionName, action);
}

void ActionRegistry::unregisterAction(
  BaseAction* action)
{
  if(action && isActionRegistered(action->getName()))
  {
    emit actionUnregistered(action->getName());
  	m_registeredActions.remove(action->getName());
  }
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

QString ActionRegistry::getActionName(
  QAction *action) const
{
  QMapIterator<QString, QAction*> ite(m_registeredActions);
  while (ite.hasNext()) 
  {
    ite.next();
    QAction *temp = (QAction *)ite.value();
    if (temp == action)
      return ite.key();
  }
  return "";
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
