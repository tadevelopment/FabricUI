/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGAddPropertyCmd_H__
#define __UI_SCENEHUB_SGAddPropertyCmd_H__
 
#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 

namespace FabricUI
{
  namespace SceneHub
  {
    const QString SGAddPropertyCmd_Str = "addPropertyCmd";
    const QString SGAddPropertyCmd_Type_Str = "SGAddPropertyCmd";

    class SGAddPropertyCmd : public SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGAddPropertyCmd(SHGLScene *shGLScene, QString cmdDes, QList<RTVal> &params, bool exec):
          SHCmd(shGLScene, SGAddPropertyCmd_Str, cmdDes, params, exec) {};
        
        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, QString command, bool exec) {
          SHCmd* cmd = 0;
          QStringList params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 2)
          {
            QString ownerPath = params[0].remove(QChar(' ')); 
            QString name = params[1].remove(QChar(' ')); 
            try 
            {
              QList<RTVal> params;
              params.append(RTVal::ConstructString(shGLScene->getClient(), ownerPath.toUtf8().constData()));
              params.append(RTVal::ConstructString(shGLScene->getClient(), name.toUtf8().constData()));
              cmd = new SGAddPropertyCmd(shGLScene, command, params, exec);
            }
            catch(Exception e)
            {
              printf("SGAddPropertyCmd::Create: exception: %s\n", e.getDesc_cstr());
            }
          }
          return cmd;
        }
 
        /// Gets the KL command parameters.
        /// \param shGLScene A reference to SHGLScene
        /// \param index The name of the object
        static QString Get(SHGLScene *shGLScene, int index) {
          QString cmd;
          try 
          {
            RTVal sgCmd = shGLScene->retrieveCmd(index);
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "ownerPath");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            QString ownerPath = QString(nameVal.getStringCString());

            keyVal = RTVal::ConstructString(shGLScene->getClient(), "name");
            nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            QString name = QString(nameVal.getStringCString());

            cmd = QString( SGAddPropertyCmd_Str + "(" + ownerPath + ", " + name + ")" );
          }
          catch(Exception e)
          {
            printf("SGAddPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return cmd;
        }
    };
  }  
}

#endif // __UI_SCENEHUB_SGAddPropertyCmd_H__


