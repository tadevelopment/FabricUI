//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FabricApplicationStates.h"

using namespace FabricUI;
using namespace Application;
using namespace FabricCore;

bool FabricApplicationStates::s_instanceFlag = false;
FabricApplicationStates* FabricApplicationStates::s_appStates = 0;

FabricApplicationStates::FabricApplicationStates(
  Client client,
  QSettings *settings)
{
  m_client = client;
  m_settings = settings;

  s_appStates = this;
  s_instanceFlag = true;
}

FabricApplicationStates::~FabricApplicationStates()
{
  s_instanceFlag = false;
}

FabricApplicationStates* FabricApplicationStates::GetAppStates() 
{
  return s_appStates;
}
 
QSettings* FabricApplicationStates::getSettings()
{
  return m_settings;
}

Client FabricApplicationStates::getClient()
{
  return m_client;
}

Context FabricApplicationStates::getContext()
{
  return m_client.getContext();
}
      
