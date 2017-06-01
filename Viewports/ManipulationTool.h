/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __MANIPULATIONTOOL_H__
#define __MANIPULATIONTOOL_H__

#include <QEvent>
#include <FabricCore.h>
#include <Commands/Command.h>
 
namespace FabricUI
{
  namespace Viewports
  {
    class ManipulationCmd : FabricServices::Commands::Command
    {

    private:
      FabricCore::RTVal m_rtval_commands;
      
    public:
      ManipulationCmd(); 
      virtual ~ManipulationCmd(); 
      
      virtual const char * getName() const { return "ManipulationCmd"; }
      virtual const char * getShortDesc() const { return "Performs any manipulation within a Fabric application."; }
      virtual const char * getFullDesc() const { return getShortDesc(); }

      static void setStaticRTValCommands(FabricCore::RTVal commands) { s_rtval_commands = commands; }

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      // We set the static commands pointer, and then construct the command. 
      static FabricCore::RTVal s_rtval_commands;
    };

    class ManipulationTool 
    {
    public:
      ManipulationTool();
      virtual ~ManipulationTool();

      virtual void toolOnSetup();
      virtual void toolOffCleanup();

      bool onEvent(QEvent *event, FabricCore::RTVal viewport, bool &redrawRequested, QString &portManipulationRequested);
      bool isActive() { return m_active;}

      FabricCore::RTVal getLastManipVal() { return m_lastManipValue; }

    private:
      bool  m_active;
      FabricCore::RTVal m_eventDispatcher;
      FabricCore::RTVal m_lastManipValue;
      FabricCore::RTVal m_lastToolValue;
    };
  };
};

#endif
