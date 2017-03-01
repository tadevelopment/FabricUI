// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "BaseAction.h"
#include "ActionRegistry.h"

using namespace FabricUI;
using namespace Actions;

void BaseAction::init(
  const QString &name, 
  const QString &text, 
  Qt::ShortcutContext context,
  bool enable,
  const QIcon &icon)
{
  m_name = name;
  setIcon(icon);
  setText(text);
  connect(this, SIGNAL(triggered()), this, SLOT(onTriggered()));
  setEnabled(enable);
  setShortcutContext(context);
  
  // Register the action inn the registry.
  ActionRegistry *registry = ActionRegistry::GetActionRegistry();
  registry->registerAction(this);
}

BaseAction::BaseAction(
  QObject *parent,
  const QString &name, 
  const QString &text, 
  QKeySequence shortcut,
  Qt::ShortcutContext context,
  bool enable,
  const QIcon &icon)
  : QAction(parent)
{ 
  // Checks if an action under this name has been 
  // registered already. If so, use its shortcuts.
  // Synchronize the shortcuts of all the actions.
  ActionRegistry *registry = ActionRegistry::GetActionRegistry();
  setShortcut( 
    (registry->getRegistrationCount(name) > 0)
    ? registry->getShortcut(name) 
    : shortcut
  );

  init(name, 
    text, 
    context, 
    enable, 
    icon);
}

BaseAction::BaseAction(
  QObject *parent,
  const QString &name, 
  const QString &text, 
  const QList<QKeySequence> & shortcuts,
  Qt::ShortcutContext context,
  bool enable,
  const QIcon &icon)
  : QAction(parent)
{
  ActionRegistry *registry = ActionRegistry::GetActionRegistry();
  setShortcuts( 
    (registry->getRegistrationCount(name) > 0)
    ? registry->getShortcuts(name) 
    : shortcuts
  );
  
  init(name, 
    text, 
    context, 
    enable, 
    icon);
}

BaseAction::~BaseAction()
{
  // Unregister our-self of the registry.
  ActionRegistry::GetActionRegistry()->unregisterAction(this);
}

QString BaseAction::getName() const
{
  return m_name;
}

void BaseAction::onTriggered()
{
}
