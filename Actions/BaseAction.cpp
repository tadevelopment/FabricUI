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
  ActionRegistry::GetActionRegistry()->registerAction(this);
  setIcon(icon);
  setText(text);
  connect(this, SIGNAL(triggered()), this, SLOT(onTriggered()));
  setEnabled(enable);
  setShortcutContext(context);
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
  init(name, 
    text, 
    context, 
    enable, 
    icon);

  setShortcut(shortcut);
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
  init(name, 
    text, 
    context, 
    enable, 
    icon);

  setShortcuts(shortcuts);
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
