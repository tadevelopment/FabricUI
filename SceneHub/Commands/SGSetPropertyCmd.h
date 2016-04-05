/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSetPropertyCmd_H__
#define __UI_SCENEHUB_SGSetPropertyCmd_H__

#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 
namespace FabricUI
{
  namespace SceneHub
  {
    const QString SGSetPropertyCmd_Str = "setPropertyCmd";
    const QString SGSetPropertyCmd_Type_Str = "SGSetPropertyCmd";

    class SGSetPropertyCmd : public SHCmd
    {
      public:   
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command description
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPropertyCmd(SHGLScene *shGLScene, QString cmdDes, QList<RTVal> &params, bool exec):
          SHCmd(shGLScene, SGSetPropertyCmd_Str, cmdDes, params, exec) {};
     
        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, QString command, bool exec) {
          SHCmd* cmd = 0;
          QStringList params;
          if(SHCmd::ExtractParams(command, params))
          {
            // Get the name of the object
            QString fullPath = params[0].remove(QChar(' ')); 
            //QString type = params[1].remove(QChar(' '));  
            //QString data = params[2].remove(QChar(' '));  
            //uint64_t dataSize = FabricUI::Util::ToNum<uint64_t>(params[3]); 
            try 
            {
              //RTVal val = SHCmd::SetParamValue(client, type.toUtf8().constData(), data);
              QList<RTVal> params;
              params.append(RTVal::ConstructString(shGLScene->getClient(), fullPath.toUtf8().constData()));
              //params[1] = val.callMethod("Type", "type", 0, 0);
              //params[2] = val.callMethod("Data", "data", 0, 0);
              //params[3] = RTVal::ConstructUInt64(client, dataSize);
              cmd = new SGSetPropertyCmd(shGLScene, command, params, exec);
            }
            catch(FabricCore::Exception e)
            {
              printf("SGSetPropertyCmd::Create: exception: %s\n", e.getDesc_cstr());
            }
          }
          return cmd;
        }

        /// Gets the KL command parameters.
        /// \param shGLScene A reference to SHGLScene
        /// \param index The name of the object
        static QString Get(SHGLScene *shGLScene, int index) {
          QString res;
          try 
          {
            RTVal sgCmd = shGLScene->retrieveCmd(index);            
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "fullPath");
            RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            QString fullPath = QString(fullPathVal.getStringCString());

            //QList<RTVal> params(3);
            //params[0] = RTVal::ConstructUInt64(shGLScene->getClient(), 0);
            //params[1] = RTVal::ConstructString(shGLScene->getClient(), string("").toUtf8().constData());
            //sgCmd.callMethod("", "getPropertyParam", 2, &params[0]);
            //uint64_t dataSize = params[0].getUInt64();
            //string type = string(params[1].getStringCString());
            //RTVal structVal = RTVal::Construct(shGLScene->getClient(), type.toUtf8().constData(), 0, 0);
            //RTVal dataVal = structVal.callMethod("Data", "data", 0, 0);
            //sgCmd.callMethod("", "getPropertyData", 1, &dataVal);
            //string data = EncodeRTValToJSON(shGLScene->getClient(), structVal);
        
            res = QString( SGSetPropertyCmd_Str + "(" + fullPath + ")" ); // ", " + type +  
              //", " + FabricUI::Util::RemoveSpace(FabricUI::Util::RemoveNewLine(data)) + ", " + FabricUI::Util::ToStr(dataSize) + ")" );
          }
          catch(FabricCore::Exception e)
          {
            printf("SGSetPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return res;
        }     
    };

  }  
}

#endif // __UI_SCENEHUB_SGSetPropertyCmd_H__
