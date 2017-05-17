
#include "Main.h"

#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/GraphViewWidget.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/ConnectionTarget.h>
#include <FabricUI/GraphView/Node.h>

using namespace FabricUI::GraphView;

Test::Controller::Controller( Graph* graph )
  : Parent( graph )
  , m_graph( graph )
{

}

Node* Test::Controller::addNode( const std::string& path )
{
  if( m_nodes.find( path ) == m_nodes.end() )
  {
    std::string name = path; // HACK
    QColor color( 64 + rand() % 128, 64 + rand() % 128, 64 + rand() % 128 );
    Node* node = new Node( m_graph, Node::NodeType::NodeType_Plain, name, name, color, color );
    m_graph->addNode( node ); // HACK ?
    m_nodes.insert( NodeMap::value_type( path, node ) );
  }
  return m_nodes[path];
}

Connection* Test::Controller::addConnection( ConnectionTarget* c1, ConnectionTarget* c2 )
{
  PathPair key( c1->path(), c2->path() );
  if( m_connections.find( key ) == m_connections.end() )
    m_connections.insert( ConnectionMap::value_type( key, new Connection( m_graph, c1, c2 ) ) );
  else
    std::cout << "connection " << c1->path() << " " << c2->path() << " already added" << std::endl;
  return m_connections[ key ];
}

std::string Test::Controller::gvcDoCopy()
{
  std::cout << "Controller::gvcDoCopy()" << std::endl;
  return std::string();
}

void Test::Controller::gvcDoPaste( bool mapPositionToMouseCursor )
{
  std::cout << "Controller::gvcDoPase()" << std::endl;
}

bool Test::Controller::gvcDoRemoveNodes( FTL::ArrayRef<Node*> nodes )
{
  std::cout << "Controller::gvcDoRemoveNodes()" << std::endl;
  return false;
}

bool Test::Controller::gvcDoAddConnections( std::vector<ConnectionTarget*> const & srcs, std::vector<ConnectionTarget*> const & dsts )
{
  for( size_t i = 0; i < srcs.size(); i++ )
    this->addConnection( srcs[i], dsts[i] );
  return false;
}

bool Test::Controller::gvcDoRemoveConnections( std::vector<ConnectionTarget*> const & srcs, std::vector<ConnectionTarget*> const & dsts )
{
  std::cout << "Controller::gvcDoRemoveConnections()" << std::endl;
  return false;
}

bool Test::Controller::gvcDoAddInstFromPreset( QString presetPath, QPointF pos )
{
  std::cout << "Controller::gvcDoAddInstFromPreset()" << std::endl;
  return false;
}

void Test::Controller::gvcDoAddPort(
  QString desiredPortName,
  PortType portType,
  QString typeSpec,
  ConnectionTarget * connectWith,
  QString extDep,
  QString metaData
)
{
  std::cout << "Controller::gvcDoAddPort() : " << desiredPortName.toUtf8().constData() << std::endl;
}

void Test::Controller::gvcDoRenameExecPort( QString oldName, QString desiredPortName, QString execPath )
{
  std::cout << "Controller::gvcDoRenameExecPort()" << std::endl;
}

void Test::Controller::gvcDoRenameNode( Node * node, QString newName )
{
  std::cout << "Controller::gvcDoRenameNode()" << std::endl;
}

void Test::Controller::gvcDoSetNodeCommentExpanded( Node * node, bool expanded )
{
  std::cout << "Controller::gvcDoSetNodeCommentExpanded()" << std::endl;
}

void Test::Controller::gvcDoMoveNodes(
  std::vector<Node*> const & nodes,
  std::vector<QPointF> const & nodesOriginalPos,
  QPointF delta,
  float gridSnapSize,
  bool allowUndo
)
{
  for( size_t i = 0; i < nodes.size(); i++ )
    nodes[i]->setPos( nodesOriginalPos[i] + delta );
}

void Test::Controller::gvcDoResizeBackDropNode( BackDropNode * backDropNode, QPointF posDelta, QSizeF sizeDelta, bool allowUndo )
{
  std::cout << "Controller::gvcDoResizeBackDropNode()" << std::endl;
}

void Test::Controller::gvcDoMoveExecPort( QString srcName, QString dstName )
{
  std::cout << "Controller::gvcDoMoveExecPort()" << std::endl;
}

QString Test::Controller::gvcGetCurrentExecPath()
{
  std::cout << "Controller::gvcGetCurrentExecPath()" << std::endl;
  return QString();
}

inline QPointF RandPos() { return QPointF( 1, 1 ) - 2 * QPointF( rand(), rand() ) / RAND_MAX ; }

void main( int argc, char** argv )
{

  QApplication* app = new QApplication( argc, argv );
  app->setStyle( new FabricUI::Style::FabricStyle() );

  // GraphView
  GraphConfig config;
  Graph* graph = new Graph( NULL, config );
  Test::Controller* controller = new Test::Controller( graph );
  graph->initialize();
  GraphViewWidget* view = new GraphViewWidget( NULL, config, graph );
  view->show();

  for( size_t i = 0; i < 100; i++ )
  {
    QString name = "Node"; name += i;
    Node* node = controller->addNode( name.toUtf8().constData() );
    node->addPin( new Pin( node, "port", PortType::PortType_IO, QColor() ) );
    node->setPos( 1000 * RandPos() );
  }

  app->exec();
}
