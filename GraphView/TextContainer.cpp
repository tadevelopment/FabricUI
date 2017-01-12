// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "TextContainer.h"
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QFontMetrics>
#include <QTextDocument>
#include <QTextCursor>
#include <QTimer>

using namespace FabricUI::GraphView;

TextContainer::TextContainer(
  QGraphicsWidget * parent,
  QString const &text,
  QColor color,
  QColor hlColor,
  QFont font,
  bool editable
  )
: QGraphicsWidget(parent),
  m_color(color),
  m_font(font),
  m_highlightColor(hlColor),
  m_text(text),
  m_highlighted(false),
  m_editable(editable),
  m_editing(false),
  m_fixedTextItem(NULL),
  m_editableTextItem(NULL)
{
  buildTextItem();

  setWindowFrameMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  refresh();
}

void TextContainer::setText(QString const &text)
{
  m_text = text;
  if( m_editing ) { m_editableTextItem->setPlainText(text); }
  else { m_fixedTextItem->setText( text + m_suffix ); }
  refresh();
}

void TextContainer::setSuffix( QString const &suffix )
{
  m_suffix = suffix;
  setText( m_text );
}

void TextContainer::refresh()
{
  QFontMetrics metrics( m_font );
  QString displayedText = m_editing ?
    m_editableTextItem->toPlainText() : m_fixedTextItem->text();
  QSize size = metrics.size( Qt::TextSingleLine, displayedText );
  prepareGeometryChange();
  setPreferredWidth(size.width());
  setPreferredHeight(size.height());
}

void TextContainer::displayedTextChanged()
{
}

QColor TextContainer::color() const
{
  return m_color;
}

QColor TextContainer::highlightColor() const
{
  return m_highlightColor;
}

void TextContainer::setColor(QColor color, QColor hlColor)
{
  m_color = color;
  m_highlightColor = hlColor;
  QColor currentColor = highlighted() ? hlColor : color;
  if ( m_editing )
  {
    // the editable item will always be highlighted
    m_editableTextItem->setDefaultTextColor( hlColor );
  }
  else
  {
    m_fixedTextItem->setBrush(currentColor);
  }
}

bool TextContainer::highlighted() const
{
  return m_highlighted;
}

void TextContainer::setHighlighted(bool state)
{
  m_highlighted = state;
  setColor(m_color, m_highlightColor);
}

QFont TextContainer::font() const
{
  return m_editing ? m_editableTextItem->font() : m_fixedTextItem->font();
}

void TextContainer::setFont(QFont font)
{
  if (m_editing) { m_editableTextItem->setFont(font); }
  else { m_fixedTextItem->setFont(font); }
  refresh();
}

bool TextContainer::italic() const
{
  return font().italic();
}

void TextContainer::setItalic(bool flag)
{
  QFont font = this->font();
  font.setItalic(flag);
  setFont(font);
}

TextContainer_EditableTextItem::TextContainer_EditableTextItem( TextContainer* container )
  : QGraphicsTextItem( container )
  , m_container(container)
  , m_displayCursor( true )
{
  // disable tabs by default : they are not allowed in most names
  setTabChangesFocus(true);

  m_timer = new QTimer( this );
  connect(
    m_timer, SIGNAL(timeout()),
    this, SLOT(cursorFlash())
    );
  m_timer->start( 500 );
}

void TextContainer_EditableTextItem::selectAllText()
{
  QTextCursor cursor = this->textCursor();
  cursor.select( QTextCursor::Document );
  this->setTextCursor( cursor );
}

void TextContainer_EditableTextItem::exit(bool submit) {
  if (submit) { m_container->submitEditedText(this->toPlainText()); }
  m_container->setEditing(false);
}

void TextContainer_EditableTextItem::focusOutEvent(QFocusEvent *event) {
  QGraphicsTextItem::focusOutEvent(event);
  exit(true); // submitting when losing focus (as ValueEditor::VELineEdit does)
}
void TextContainer_EditableTextItem::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
  case Qt::Key_Escape:
    exit(false); break;
  case Qt::Key_Enter:
  case Qt::Key_Return:
    exit(true); break;
  default:
    m_displayCursor = true;
    update();
    m_timer->start();
    QGraphicsTextItem::keyPressEvent(event);
    m_container->refresh();
    m_container->displayedTextChanged();
  }
}

void TextContainer_EditableTextItem::paint(
  QPainter * painter,
  const QStyleOptionGraphicsItem * option,
  QWidget * widget
  )
{
  QTextCursor cursor = this->textCursor();
  QTextDocument *document = this->document();
  QAbstractTextDocumentLayout *layout = document->documentLayout();

  QRectF rect = layout->frameBoundingRect( document->rootFrame() );

  // Fill background
  painter->fillRect( rect, Qt::white );

  // Draw text with selection and cursor
  QAbstractTextDocumentLayout::PaintContext context;
  context.palette.setColor( QPalette::Text, Qt::black );
  if ( cursor.hasSelection() )
  {
    QAbstractTextDocumentLayout::Selection selection;
    selection.cursor = cursor;
    selection.format.setForeground( Qt::white );
    selection.format.setBackground( Qt::darkBlue );
    context.selections.append( selection );
  }
  else if ( m_displayCursor )
    context.cursorPosition = cursor.position();
  layout->draw( painter, context );

  // Draw black frame
  painter->setPen( Qt::black );
  painter->drawRect( rect );
}

void TextContainer_EditableTextItem::cursorFlash()
{
  m_displayCursor = !m_displayCursor;
  update();
}

void TextContainer::buildTextItem()
{
  destroyTextItems();
  if (m_editing)
  {
    TextContainer_EditableTextItem* editableTextItem = new TextContainer_EditableTextItem( this );
    m_editableTextItem = editableTextItem;
    m_editableTextItem->setTextInteractionFlags( Qt::TextEditorInteraction );
    m_editableTextItem->setCacheMode(DeviceCoordinateCache);

    // make it look the same as QGraphicsSimpleTextItem
    QTextDocument *document = m_editableTextItem->document();
    document->setDocumentMargin(0);

    m_editableTextItem->setFocus();
    setText( m_text );

    // select all the text when entering edit mode
    editableTextItem->selectAllText();
  }
  else
  {
    m_fixedTextItem = new QGraphicsSimpleTextItem( this );
    m_fixedTextItem->setCacheMode(DeviceCoordinateCache);
    setText( m_text );
  }
  setColor( m_color, m_highlightColor );
  setFont( m_font );
}

TextContainer::~TextContainer() {
  destroyTextItems();
}

void TextContainer::destroyTextItems()
{
  if (m_fixedTextItem != NULL) { delete m_fixedTextItem; m_fixedTextItem = NULL; }
  if (m_editableTextItem != NULL) { delete m_editableTextItem; m_editableTextItem = NULL; }
}

void TextContainer::setEditing(bool editing) {
  
  if ( m_editing != editing) {
    m_editing = editing;
    buildTextItem();
    displayedTextChanged();
  }
}
