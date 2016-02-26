//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <QtCore/QSharedPointer>
#include <QtGui/QWidget>
#include <FTL/ArrayRef.h>
#include <FTL/CStrRef.h>

class BaseModelItem;
class VETreeWidget;

namespace FTL {
  class JSONArray;
}

namespace FabricUI {

  namespace DFG {
    class DFGController;
    class DFGWidget;
  }
  namespace GraphView {
    class Node;
    class Graph;
  }
  namespace ModelItems {
    class BindingModelItem;
    class NodeModelItem;
    class RootModelItem;
  }

  namespace ValueEditor 
  { 

    class VEEditorOwner : public QObject 
    {
      Q_OBJECT

    public:

      VEEditorOwner();
      ~VEEditorOwner();

      virtual QWidget* getWidget() const;
      virtual void initConnections();

    public slots :
      virtual void onOutputsChanged() {} // Call after each evaluation

    protected slots:
      virtual void onFrameChanged(int frame) {}

    signals:
      void log(const char * message);
      void replaceModelRoot( BaseModelItem* model);
      void modelItemInserted( BaseModelItem* parent, int index, const char* childName);
      void modelItemTypeChange( BaseModelItem* changingItem, const char* newType);
      void modelItemRemoved( BaseModelItem* removedItem);
      void modelItemChildrenReordered( BaseModelItem* parent, const QList<int>& newOrder );
      void modelItemRenamed( BaseModelItem* renamedItem );

    protected:

      VETreeWidget* m_valueEditor;
      FabricUI::ModelItems::RootModelItem* m_modelRoot;
    };
}
}
