/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
#define __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
 
#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 
namespace FabricUI
{
  namespace SceneHub
  {
    const QString SGSetPaintToolAttributeCmd_Str = "setPaintToolAttributeCmd";
    const QString SGSetPaintToolAttributeCmd_Type_Str = "SGSetPaintToolAttributeCmd";

    class SGSetPaintToolAttributeCmd : public SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPaintToolAttributeCmd(SHGLScene *shGLScene, QString cmdDes, QList<RTVal> &params, bool exec):
          SHCmd(shGLScene, SGSetPaintToolAttributeCmd_Str, cmdDes, params, exec) {};
      
        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, QString command, bool exec) {
          // Not scriptable
          if(exec == true) 
            return 0;

          SHCmd* cmd = 0;
          QStringList params;
          if(SHCmd::ExtractParams(command, params))
          {
            // Get the name of the object
            QString fullPath = params[0].remove(QChar(' ')); 
            try 
            {
              QList<RTVal> params;
              params.append(RTVal::ConstructString(shGLScene->getClient(), fullPath.toUtf8().constData()));
              cmd = new SGSetPaintToolAttributeCmd(shGLScene, command, params, exec);
            }
            catch(Exception e)
            {
              printf("SGSetPaintToolAttributeCmd::Create: exception: %s\n", e.getDesc_cstr());
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
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "fullPath");
            RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            QString fullPath = QString(fullPathVal.getStringCString());
            cmd = QString( SGSetPaintToolAttributeCmd_Str + "(" + fullPath + ")" );
          }
          catch(Exception e)
          {
            printf("SGSetPaintToolAttributeCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return cmd;
        }
        
    };
  }  
}

#endif // __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
