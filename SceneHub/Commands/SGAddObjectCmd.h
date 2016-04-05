/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGAddObjectCmd_H__
#define __UI_SCENEHUB_SGAddObjectCmd_H__

#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 
namespace FabricUI
{
  namespace SceneHub
  {
    const QString SGAddObjectCmd_Str = "addObjectCmd";
    const QString SGAddObjectCmd_Type_Str = "SGAddObjectCmd";

    class SGAddObjectCmd : public SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGAddObjectCmd(SHGLScene *shGLScene, QString cmdDes, QList<RTVal> &params, bool exec):
          SHCmd(shGLScene, SGAddObjectCmd_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, QString command, bool exec) {
          SHCmd* cmd = 0;
          QStringList params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 2)
          {
            // Get the name of the object
            QString name = params[0].remove(QChar(' '));
            QString isGlobalStr = params[1].remove(QChar(' '));
            // Get if it a global object
            try 
            {
              QList<RTVal> params;
              params.append(RTVal::ConstructString(shGLScene->getClient(), name.toUtf8().constData()));
              params.append(RTVal::ConstructBoolean(shGLScene->getClient(), isGlobalStr.toLower() == "true"));
              cmd = new SGAddObjectCmd(shGLScene, command, params, exec);
            }
            catch(Exception e)
            {
              printf("SGAddObjectCmd::Create: exception: %s\n", e.getDesc_cstr());
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
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "name");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            QString name = QString(nameVal.getStringCString());

            keyVal = RTVal::ConstructString(shGLScene->getClient(), "isGlobal");
            bool isGlobal = sgCmd.callMethod("Boolean", "getBooleanParam", 1, &keyVal).getBoolean();
            cmd = QString( SGAddObjectCmd_Str + "(" + name + ", " + QString(isGlobal) + ")" );
          }
          catch(Exception e)
          {
            printf("SGAddObjectCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return cmd;
        }
        
    };
  }  
}

#endif // __UI_SCENEHUB_SGAddObjectCmd_H__


