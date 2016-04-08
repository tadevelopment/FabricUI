/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHCmd.h"
using namespace FabricUI;
using namespace SceneHub;
 
SHCmd::SHCmd() 
  : m_state(State_New)
  , m_coreUndoCount(0) {
}

SHCmdDescription SHCmd::registerCommand() { 
  return m_desctiption; 
}

QString SHCmd::getFromRTVal(FabricCore::RTVal cmd) { 
  QString str; 
  return str; 
}

void SHCmd::setScene(SHGLScene *scene) {
  m_shGLScene = scene;
}

QString SHCmd::getCommand() { 
  assert(wasInvoked()); 
  return m_command; 
}

void SHCmd::setCommand(QString command) { 
  m_command = command; 
}

void SHCmd::doit() {
  assert(m_state == State_New);
  ++m_coreUndoCount;
  m_state = State_Done;
}

void SHCmd::undo() {
  assert(m_state == State_Done || m_state == State_Redone);
  m_state = State_Undone;
  m_shGLScene->undoCmd(m_coreUndoCount);
}

void SHCmd::redo() {
  assert( m_state = State_Undone );
  m_state = State_Redone; 
  m_shGLScene->redoCmd(m_coreUndoCount);
}

bool SHCmd::wasInvoked() { 
  return m_state != State_New; 
}

void SHCmd::execute(QString command) {
  m_command = command;
}

QStringList SHCmd::extractParams(QString command) {
  QStringList params;
  QStringList split = command.split("(");
  if(split.size() == 2)
  {
    QStringList split_ = split[1].split(")");
    if(split_.size() == 2)
    {             
      QString paramArray = split_[0].remove(QChar(' '));

      // Parameter like {...,...,...}
      int begin = paramArray.indexOf("{");
      int end = paramArray.lastIndexOf("}");
    
      if(begin < end)
      {
        // Get the index to insert the {} params
        QString temp = paramArray.mid(0, begin);
        int index = temp.count(QChar(','));

        // Get the {} params string
        QString params_ = paramArray.mid(begin, end-begin+1);
        // Get the {} params string
        int index2 = paramArray.mid(end, paramArray.length()-1).indexOf(",");
        paramArray.remove(begin, end+index2+1-begin);
        
        // Construct the vector of parameters
        QStringList tempArray = paramArray.split(",");
        for(int i=0; i<index; ++i) 
          params.append(tempArray[i]);

        params.append(params_);

        for(int i=index; i<tempArray.size(); ++i) 
          params.append(tempArray[i]);
      }
      else 
      {
        params = paramArray.split(",");
        if(params.size() == 0 && paramArray != "")
          params.append(paramArray);
      }
    }
  }
  return params;
}
