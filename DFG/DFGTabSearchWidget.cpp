// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGTabSearchWidget.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>

#include <QCursor>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGBaseTabSearchWidget::DFGBaseTabSearchWidget(
  DFGWidget * parent,
  const DFGConfig & config
  )
  : QWidget(parent)
  , m_parent( parent )
  , m_config( config )
  , m_queryMetrics( config.searchQueryFont )
  , m_resultsMetrics( config.searchResultsFont )
  , m_helpMetrics( config.searchHelpFont )
{
  // always show on top
  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
  setMouseTracking(true);
}

DFGBaseTabSearchWidget::~DFGBaseTabSearchWidget()
{
  releaseKeyboard();
}

void DFGBaseTabSearchWidget::mousePressEvent(QMouseEvent * event)
{
  // If we get a left click
  if(event->button() == Qt::LeftButton)
  {
    // Get the element index from the click pos
    int index = indexFromPos( event->pos() );
    if ( index >= 0 && index < int( resultsSize() ) )
    {
      // Then add the node to the graph
      addNodeForIndex( index );
      m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
      event->accept();
      return;
    }
  }
  QWidget::mousePressEvent(event);
}

void DFGBaseTabSearchWidget::mouseMoveEvent( QMouseEvent *event )
{
  int index = indexFromPos( event->pos() );
  if ( m_currentIndex != index )
  {
    m_currentIndex = index;
    update();
  }
  event->accept();
}

void DFGBaseTabSearchWidget::keyPressEvent(QKeyEvent * event)
{
  Qt::Key key = (Qt::Key)event->key();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  // Do nothing if control or alt is pressed
  if(modifiers.testFlag(Qt::ControlModifier) || modifiers.testFlag(Qt::AltModifier))
    event->accept();
 
  else if(key == Qt::Key_Tab || key == Qt::Key_Escape)
  {
    hide();
    event->accept();
    m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
  }
  // alphanumeric or period
  else if( acceptKey( key ) )
  {
    m_search += event->text();
    updateSearch();
    event->accept();
  }
  else if(key == Qt::Key_Backspace)
  {
    if(m_search.length() > 0)
    {
      m_search = m_search.left(m_search.length()-1);
      updateSearch();
    }
    event->accept();
  }
  else if(key == Qt::Key_Up)
  {
    if(m_currentIndex > 0)
    {
      m_currentIndex--;
      update();
    }
    event->accept();
  }
  else if(key == Qt::Key_Down)
  {
    if ( m_currentIndex < int( resultsSize() ) - 1 )
    {
      m_currentIndex++;
      update();
    }
    event->accept();
  }
  else if(key == Qt::Key_Enter || key == Qt::Key_Return)
  {
    if(m_currentIndex > -1 && m_currentIndex < int( resultsSize() ))
    {
      addNodeForIndex( m_currentIndex );
    }
    hide();
    event->accept();
    m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
  }
  else
  {
    QWidget::keyPressEvent(event);
  }
}

char const *DFGBaseTabSearchWidget::getHelpText() const
{
  if ( m_search.length() == 0 )
    return "Search for preset or variable";
  else if ( resultsSize() == 0 )
    return "No results found";
  else
    return 0;
}

void DFGBaseTabSearchWidget::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);

  int width = widthFromResults();
  int height = heightFromResults();

  painter.fillRect(0, 0, width, height, m_config.searchBackgroundColor);
  painter.fillRect(
    margin() + m_queryMetrics.width( m_search ),
    margin(),
    m_queryMetrics.width( 'X' ),
    m_queryMetrics.lineSpacing(),
    m_config.searchCursorColor
    );

  if(m_currentIndex > -1 && m_currentIndex < int( resultsSize() ) )
  {
    int offset = m_resultsMetrics.lineSpacing() * (m_currentIndex + 1) + margin();
    painter.fillRect(margin(), offset, width - 2 * margin(), m_resultsMetrics.lineSpacing(), m_config.searchHighlightColor);
  }

  painter.setPen(m_config.searchBackgroundColor.darker());
  painter.drawRect(0, 0, width-1, height-1);

  int offset = margin() + m_queryMetrics.ascent();
  painter.setFont(m_config.searchQueryFont);
  painter.setPen(QColor(0, 0, 0, 255));
  painter.drawText(margin(), offset, m_search);
  offset += m_queryMetrics.lineSpacing();

  painter.setFont(m_config.searchResultsFont);
  for(int i=0;i<int( resultsSize() );i++)
  {
    painter.drawText(margin(), offset, resultLabel(i));
    offset += m_resultsMetrics.lineSpacing();
  }

  if ( char const *helpText = getHelpText() )
  {
    painter.setFont(m_config.searchHelpFont);
    painter.drawText(margin(), offset, helpText);
    offset += m_helpMetrics.lineSpacing();
  }

  QWidget::paintEvent(event);  
}

void DFGBaseTabSearchWidget::hideEvent(QHideEvent * event)
{
  releaseKeyboard();
  clear();
  emit enabled(false);
  QWidget::hideEvent(event);  
}

void DFGBaseTabSearchWidget::showForSearch( QPoint globalPos )
{
  clear();
  m_search.clear();
  m_currentIndex = -1;
  setFocus(Qt::TabFocusReason);

  m_originalLocalPos = m_parent->mapFromGlobal( globalPos );
  m_originalLocalPos -= QPoint( width() / 2, m_queryMetrics.lineSpacing() / 2);
  setGeometry( m_originalLocalPos.x(), m_originalLocalPos.y(), 0, 0 );
  updateGeometry();

  emit enabled(true);
  show();

  grabKeyboard();
}

void DFGBaseTabSearchWidget::showForSearch()
{
  showForSearch(QCursor::pos());
}

void DFGBaseTabSearchWidget::focusOutEvent(QFocusEvent * event)
{
  hide();
}

bool DFGBaseTabSearchWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}

void DFGBaseTabSearchWidget::updateSearch()
{
  if( resultsSize() == 0)
  {
    m_currentIndex = -1;
  }
  else if(m_currentIndex == -1)
  {
    m_currentIndex = 0;
  }
  else if(m_currentIndex >= int( resultsSize() ))
  {
    m_currentIndex = 0;
  }

  updateGeometry();
}

bool DFGBaseTabSearchWidget::acceptKey( const Qt::Key key ) const
{
  return ( int( key ) >= int( Qt::Key_0 ) && int( key ) <= int( Qt::Key_9 ) ) ||
    ( int( key ) >= int( Qt::Key_A ) && int( key ) <= int( Qt::Key_Z ) ) ||
    key == Qt::Key_Period || key == Qt::Key_Underscore;
}

void DFGLegacyTabSearchWidget::updateSearch()
{
  m_results =
    m_parent->getUIController()->getPresetPathsFromSearch(
      m_search.toUtf8().constData()
    );
  m_results.keepFirst( 16 );

  DFGBaseTabSearchWidget::updateSearch();
}

FTL::StrRef DFGLegacyTabSearchWidget::getName( unsigned int index ) const
{
  return static_cast<char const *>( m_results.getUserdata( index ) );
}

bool DFGTabSearchWidget::acceptKey( const Qt::Key key ) const
{
  return DFGBaseTabSearchWidget::acceptKey( key )
    || key == Qt::Key_Space;
}

void DFGTabSearchWidget::updateSearch()
{
  // Splitting the search string into a char**
  const std::string searchStr = m_search.toUtf8().constData();

  std::vector<std::string> tagsStr;
  unsigned int start = 0;
  for( unsigned int end = 0; end < searchStr.size(); end++ )
  {
    const char c = searchStr[end];
    if( c == '.' || c == ' ' ) // delimiters
    {
      if( end - start > 0 )
        tagsStr.push_back( searchStr.substr( start, end - start ) );
      start = end+1;
    }
  }
  if( start < searchStr.size() )
    tagsStr.push_back( searchStr.substr( start, searchStr.size() - start ) );

  std::vector<char const*> tags( tagsStr.size() );

  // Debug : TODO remove
  for( unsigned int i = 0; i < tagsStr.size(); i++ )
    std::cout << "\"" << tagsStr[i] << "\" ";
  std::cout << std::endl;

  for( unsigned int i = 0; i < tagsStr.size(); i++ )
    tags[i] = tagsStr[i].data();

  // Querying the DataBase of presets
  FabricCore::DFGHost& host = m_parent->getUIController()->getHost();
  FEC_StringRef jsonStr = FEC_DFGHostSearchPresets(
    host.getFECDFGHostRef(),
    tags.size(),
    tags.data(),
    0,
    16
  );
  FTL::StrRef jsonStrR( FEC_StringGetCStr( jsonStr ), FEC_StringGetSize( jsonStr ) );
  const FTL::JSONValue* json = FTL::JSONValue::Decode( jsonStrR );
  const FTL::JSONObject* root = json->cast<FTL::JSONObject>();
  const FTL::JSONArray* results = root->getArray("results");
  m_results.resize( results->size() );
  for( unsigned int i = 0; i < results->size(); i++ )
    m_results[i] = results->getArray( i )->getString( 0 );

  DFGBaseTabSearchWidget::updateSearch();
}

DFGTabSearchWidget::DFGTabSearchWidget( DFGWidget * parent, const DFGConfig & config )
  : DFGBaseTabSearchWidget( parent, config )
{
  // HACK : give a different look to this widget
  m_config.searchBackgroundColor = QColor( 180, 190, 210 );
  m_config.searchHighlightColor = QColor( 255, 225, 200 );
}

FTL::StrRef DFGTabSearchWidget::getName( unsigned int index ) const
{
  return m_results[index];
}


int DFGBaseTabSearchWidget::margin() const
{
  return 2;
}

void DFGBaseTabSearchWidget::updateGeometry()
{
  QRect rect = geometry();
  int width = widthFromResults();
  int height = heightFromResults();

  QPoint localPos = m_originalLocalPos;

  // ensure the widget is properly positioned.
  QWidget * parentWidget = qobject_cast<QWidget*>(parent());
  if(parentWidget)
  {
    // correct the x position.
    if (width >= parentWidget->width())
    {
      localPos.setX(0);
    }
    else
    {
      if (localPos.x() < 0)
        localPos.setX(0);
      else if (localPos.x() + width >= parentWidget->width())
        localPos.setX(parentWidget->width() - width);
    }

    // correct the y position.
    if (height >= parentWidget->height())
    {
      localPos.setY(0);
    }
    else
    {
      if (localPos.y() < 0)
        localPos.setY(0);
      else if (localPos.y() + height >= parentWidget->height())
        localPos.setY(parentWidget->height() - height);
    }
  }

  rect.setTopLeft( localPos );
  rect.setSize( QSize( width, height ) );

  setGeometry( rect );
  update();
}

QString DFGBaseTabSearchWidget::resultLabel(unsigned int index) const
{
  FTL::StrRef desc = getName( index );
  FTL::StrRef::Split splitOne = desc.rsplit('.');
  if ( !splitOne.second.empty() )
  {
    FTL::StrRef::Split splitTwo = splitOne.first.rsplit('.');
    if ( !splitTwo.first.empty()
      && !splitTwo.second.empty() )
    {
      QString result = "...";
      result += QString::fromUtf8( splitTwo.second.data(), splitTwo.second.size() );
      result += '.';
      result += QString::fromUtf8( splitOne.second.data(), splitOne.second.size() );
      return result;
    }
  }
  return QString::fromUtf8( desc.data(), desc.size() );
}

int DFGBaseTabSearchWidget::indexFromPos(QPoint pos)
{
  int y = pos.y();
  y -= margin();
  y -= m_queryMetrics.lineSpacing();
  y /= (int)m_resultsMetrics.lineSpacing();
  if ( y < 0 || y >= int( resultsSize() ) )
    return -1;
  return y;
}

int DFGBaseTabSearchWidget::widthFromResults() const
{
  int width = 80;

  int w = m_queryMetrics.width(m_search) + m_queryMetrics.width('X');
  if(w > width)
    width = w;

  for(int i=0;i<int( resultsSize() );i++)
  {
    w = m_resultsMetrics.width(resultLabel(i));
    if(w > width)
      width = w;
  }

  if ( char const *helpText = getHelpText() )
  {
    w = m_helpMetrics.width( helpText );
    if ( w > width )
      width = w;
  }

  return width + 2 * margin();
}

int DFGBaseTabSearchWidget::heightFromResults() const
{
  int height = m_queryMetrics.lineSpacing();
  height += resultsSize() * m_resultsMetrics.lineSpacing();
  if ( getHelpText() )
    height += m_helpMetrics.lineSpacing();
  return height + 2 * margin();
}

void DFGBaseTabSearchWidget::addNodeForIndex( unsigned index )
{
  DFGController *controller = m_parent->getUIController();
  
  QPoint localPos = geometry().topLeft();
  QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);

  // init node name.
  QString nodeName;

  FTL::StrRef desc = getName( index );

  // deal with special case
  if ( desc == FTL_STR("var") )
  {
    FabricCore::Client client = controller->getClient();
    FabricCore::DFGBinding binding = controller->getBinding();

    DFGNewVariableDialog dialog(
      this, client, binding, controller->getExecPath()
      );
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString name = dialog.name();
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();

    if (name.isEmpty())
    { controller->log("Warning: no variable created (empty name).");
      return; }
    if (dataType.isEmpty())
    { controller->log("Warning: no variable created (empty type).");
      return; }

    nodeName = controller->cmdAddVar(
      name.toUtf8().constData(), 
      dataType.toUtf8().constData(), 
      extension.toUtf8().constData(), 
      scenePos
      );
  }
  else if( desc == FTL_STR("get") )
  {
    nodeName = controller->cmdAddGet(
      "get",
      "",
      scenePos
      );
  }
  else if( desc == FTL_STR("set") )
  {
    nodeName = controller->cmdAddSet(
      "set",
      "",
      scenePos
      );
  }
  else if( desc.startswith( FTL_STR("get.") ) )
  {
    FTL::StrRef varName = desc.drop_front( 4 );
    nodeName = controller->cmdAddGet(
      "get",
      QString::fromUtf8( varName.data(), varName.size() ),
      scenePos
      );
  }
  else if( desc.startswith( FTL_STR("set.") ) )
  {
    FTL::StrRef varName = desc.drop_front( 4 );
    nodeName = controller->cmdAddSet(
      "set",
      QString::fromUtf8( varName.data(), varName.size() ),
      scenePos
      );
  }
  else if( desc == FTL_STR("backdrop") )
  {
    controller->cmdAddBackDrop(
      "backdrop",
      scenePos
      );
  }
  else
  {
    select( index );
    nodeName = controller->cmdAddInstFromPreset(
      QString::fromUtf8( desc.data(), desc.size() ),
      scenePos
      );
  }

  // was a new node created?
  if ( !nodeName.isEmpty() )
  {
    m_parent->getGraphViewWidget()->graph()->clearSelection();
    if ( GraphView::Node *uiNode = m_parent->getGraphViewWidget()->graph()->node( nodeName ) )
      uiNode->setSelected( true );
  }
}
