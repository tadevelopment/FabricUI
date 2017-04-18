#pragma once

#include <iostream>

#include <QApplication>
#include <FabricUI/Style/FabricStyle.h>
#include <FabricUI/GraphView/GraphViewWidget.h>

namespace FabricUI
{
namespace GraphView
{
namespace Test
{
  class Controller : public FabricUI::GraphView::Controller
  {
    typedef FabricUI::GraphView::Controller Parent;

  public:
    Controller( Graph * graph );

    // Inherited via Controller
    virtual std::string gvcDoCopy() FTL_OVERRIDE;
    virtual void gvcDoPaste( bool mapPositionToMouseCursor = true ) FTL_OVERRIDE;
    virtual bool gvcDoRemoveNodes( FTL::ArrayRef<Node*> nodes ) FTL_OVERRIDE;
    virtual bool gvcDoAddConnections(
      std::vector<ConnectionTarget*> const & srcs,
      std::vector<ConnectionTarget*> const & dsts
    ) FTL_OVERRIDE;
    virtual bool gvcDoRemoveConnections(
      std::vector<ConnectionTarget*> const & srcs,
      std::vector<ConnectionTarget*> const & dsts
    ) FTL_OVERRIDE;
    virtual bool gvcDoAddInstFromPreset( QString presetPath, QPointF pos ) FTL_OVERRIDE;
    virtual void gvcDoAddPort(
      QString desiredPortName,
      PortType portType,
      QString typeSpec = QString(),
      ConnectionTarget * connectWith = 0,
      QString extDep = QString(),
      QString metaData = QString()
    ) FTL_OVERRIDE;
    virtual void gvcDoRenameExecPort(
      QString oldName,
      QString desiredPortName,
      QString execPath = ""
    ) FTL_OVERRIDE;
    virtual void gvcDoRenameNode( Node * node, QString newName ) FTL_OVERRIDE;
    virtual void gvcDoSetNodeCommentExpanded( Node * node, bool expanded ) FTL_OVERRIDE;
    virtual void gvcDoMoveNodes(
      std::vector<Node*> const & nodes,
      std::vector<QPointF> const & nodesOriginalPos,
      QPointF delta,
      float gridSnapSize,
      bool allowUndo
    ) FTL_OVERRIDE;
    virtual void gvcDoResizeBackDropNode(
      BackDropNode * backDropNode,
      QPointF posDelta,
      QSizeF sizeDelta,
      bool allowUndo
    ) FTL_OVERRIDE;
    virtual void gvcDoMoveExecPort( QString srcName, QString dstName ) FTL_OVERRIDE;
    virtual QString gvcGetCurrentExecPath() FTL_OVERRIDE;

    Node* addNode( const std::string& path );
    Connection* addConnection( ConnectionTarget*, ConnectionTarget* );

  private:
    Graph* m_graph;
    typedef std::map<std::string, Node*> NodeMap;
    NodeMap m_nodes;
    struct PathPair
    {
      std::string src, dst;
      PathPair( const std::string& src, const std::string& dst ) : src( src ), dst( dst ) {}
      bool operator==( const PathPair& p ) const { return ( src == p.src && dst == p.dst ) || ( src == p.dst && dst == p.src ); }
      bool operator<( const PathPair& p ) const { return !(( *this ) == p ) && src < p.src; } // HACK ?
    };
    typedef std::map<PathPair, Connection*> ConnectionMap;
    ConnectionMap m_connections;
  };

}
}
}
