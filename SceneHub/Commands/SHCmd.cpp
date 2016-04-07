/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHCmd.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;
 

bool SHCmd::ExtractParams(QString command, QStringList &params) {
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

      return true;
    }
  }
  return false;
}

bool SHCmd::ExtractName(QString command, QString &name) {
  QStringList split = command.split("(");
  if(split.size() > 1) 
  {
    name = split[0];
    return true;
  }
  return false;
}

SHCmd::SHCmd(
  SHGLScene *shGLScene,
  QString cmdName, 
  QString cmdDes, 
  QList<FabricCore::RTVal> &params, 
  bool exec) 
  : m_state(State_New)
  ,  m_coreUndoCount(0) 
  ,  m_shGLScene(shGLScene) 
{
  setDesc(cmdDes);
  // !!! executeCmd(QString cmdName, std::vector<RTVal> &params)
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

void SHCmd::addRTValDependency(FabricCore::RTVal val) {
  m_additionalRTVals.append(val);
}
