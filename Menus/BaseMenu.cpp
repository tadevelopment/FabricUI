/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "BaseMenu.h"
 
using namespace FabricCore;
using namespace FabricUI;
using namespace Menus;

BaseMenu::BaseMenu(
  Client client, 
  QString title, 
  QWidget *parent)
  : QMenu(parent)
  , m_client(client) {
  if(!title.isEmpty())
    setTitle(title);
}

BaseMenu::~BaseMenu() {}

FabricCore::Client BaseMenu::getClient() {
  return m_client;
}

void BaseMenu::constructMenu() {}
