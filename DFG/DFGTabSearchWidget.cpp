// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGTabSearchWidget.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>

#include <QCursor>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGTabSearchWidget::DFGTabSearchWidget(
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

DFGTabSearchWidget::~DFGTabSearchWidget()
{
  releaseKeyboard();
}

void DFGTabSearchWidget::mousePressEvent(QMouseEvent * event)
{
  // If we get a left click
  if(event->button() == Qt::LeftButton)
  {
    // Get the element index from the click pos
    int index = indexFromPos( event->pos() );
    if ( index >= 0 && index < int(m_results.getSize()) )
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

void DFGTabSearchWidget::mouseMoveEvent( QMouseEvent *event )
{
  int index = indexFromPos( event->pos() );
  if ( m_currentIndex != index )
  {
    m_currentIndex = index;
    update();
  }
  event->accept();
}

void DFGTabSearchWidget::keyPressEvent(QKeyEvent * event)
{
  Qt::Key key = (Qt::Key)event->key();
  if(key == Qt::Key_Tab || key == Qt::Key_Escape)
  {
    hide();
    event->accept();
    m_parent->getGraphViewWidget()->setFocus(Qt::OtherFocusReason);
  }
  // alphanumeric or period
  else if((int(key) >= int(Qt::Key_0) && int(key) <= int(Qt::Key_9)) ||
    (int(key) >= int(Qt::Key_A) && int(key) <= int(Qt::Key_Z)) ||
    key == Qt::Key_Period || key == Qt::Key_Underscore)
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
    if ( m_currentIndex < int(m_results.getSize()) - 1 )
    {
      m_currentIndex++;
      update();
    }
    event->accept();
  }
  else if(key == Qt::Key_Enter || key == Qt::Key_Return)
  {
    if(m_currentIndex > -1 && m_currentIndex < int(m_results.getSize()))
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

char const *DFGTabSearchWidget::getHelpText() const
{
  if ( m_search.length() == 0 )
    return "Search for preset or variable";
  else if ( m_results.getSize() == 0 )
    return "No results found";
  else
    return 0;
}

void DFGTabSearchWidget::paintEvent(QPaintEvent * event)
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

  if(m_currentIndex > -1 && m_currentIndex < int(m_results.getSize()) )
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
  for(int i=0;i<int(m_results.getSize());i++)
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

void DFGTabSearchWidget::hideEvent(QHideEvent * event)
{
  releaseKeyboard();
  m_results.clear();
  emit enabled(false);
  QWidget::hideEvent(event);  
}

void DFGTabSearchWidget::showForSearch( QPoint globalPos )
{
  m_results.clear();
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

void DFGTabSearchWidget::showForSearch()
{
  showForSearch(QCursor::pos());
}

void DFGTabSearchWidget::focusOutEvent(QFocusEvent * event)
{
  hide();
}

bool DFGTabSearchWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}

void DFGTabSearchWidget::updateSearch()
{
  m_results =
    m_parent->getUIController()->getPresetPathsFromSearch(
      m_search.toUtf8().constData()
      );
  m_results.keepFirst( 16 );

  if(m_results.getSize() == 0)
  {
    m_currentIndex = -1;
  }
  else if(m_currentIndex == -1)
  {
    m_currentIndex = 0;
  }
  else if(m_currentIndex >= int(m_results.getSize()))
  {
    m_currentIndex = 0;
  }

  updateGeometry();
}

int DFGTabSearchWidget::margin() const
{
  return 2;
}

void DFGTabSearchWidget::updateGeometry()
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

QString DFGTabSearchWidget::resultLabel(unsigned int index) const
{
  FTL::StrRef desc(
    static_cast<char const *>( m_results.getUserdata(index) )
    );
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

int DFGTabSearchWidget::indexFromPos(QPoint pos)
{
  int y = pos.y();
  y -= margin();
  y -= m_queryMetrics.lineSpacing();
  y /= (int)m_resultsMetrics.lineSpacing();
  if ( y < 0 || y >= int(m_results.getSize()) )
    return -1;
  return y;
}

int DFGTabSearchWidget::widthFromResults() const
{
  int width = 80;

  int w = m_queryMetrics.width(m_search) + m_queryMetrics.width('X');
  if(w > width)
    width = w;

  for(int i=0;i<int(m_results.getSize());i++)
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

int DFGTabSearchWidget::heightFromResults() const
{
  int height = m_queryMetrics.lineSpacing();
  height += m_results.getSize() * m_resultsMetrics.lineSpacing();
  if ( getHelpText() )
    height += m_helpMetrics.lineSpacing();
  return height + 2 * margin();
}

void DFGTabSearchWidget::addNodeForIndex( unsigned index )
{
  DFGController *controller = m_parent->getUIController();
  
  QPoint localPos = geometry().topLeft();
  QPointF scenePos = m_parent->getGraphViewWidget()->graph()->itemGroup()->mapFromScene(localPos);

  // init node name.
  QString nodeName;

  FTL::CStrRef desc( static_cast<char const *>( m_results.getUserdata( index ) ) );

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
    m_results.select( index );
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
