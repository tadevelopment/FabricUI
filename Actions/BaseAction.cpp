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
  ActionRegistry::GetActionRegistry()->registerAction(this);
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
  setShortcut(shortcut);

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
  setShortcuts(shortcuts);

  init(name, 
    text, 
    context, 
    enable, 
    icon);
}

BaseAction::~BaseAction()
{
  ActionRegistry::GetActionRegistry()->unregisterAction(this);
}

QString BaseAction::getName() const
{
  return m_name;
}

void BaseAction::onTriggered()
{
}
