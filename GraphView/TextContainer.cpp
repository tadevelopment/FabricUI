// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "TextContainer.h"
#include <QAbstractTextDocumentLayout>
#include <QPen>
#include <QFontMetrics>
#include <QTextDocument>
#include <QTextCursor>

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

class TextContainer::EditableTextItem : public QGraphicsTextItem {

  typedef QGraphicsTextItem Parent;

  TextContainer* m_container;

public:
  EditableTextItem( TextContainer* container )
    : QGraphicsTextItem( container ),
    m_container(container)
  {
    // disable tabs by default : they are not allowed in most names
    setTabChangesFocus(true);
  }

  void selectAllText()
  {
    QTextCursor cursor = this->textCursor();
    cursor.select( QTextCursor::Document );
    this->setTextCursor( cursor );
  }

private:
  void exit(bool submit) {
    if (submit) { m_container->submitEditedText(this->toPlainText()); }
    m_container->setEditing(false);
  }

protected:
  void focusOutEvent(QFocusEvent *event) {
    QGraphicsTextItem::focusOutEvent(event);
    exit(true); // submitting when losing focus (as ValueEditor::VELineEdit does)
  }
  void keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Escape:
      exit(false); break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      exit(true); break;
    default:
      QGraphicsTextItem::keyPressEvent(event);
      m_container->refresh();
      m_container->displayedTextChanged();
    }
  }

  virtual void paint(
    QPainter * painter,
    const QStyleOptionGraphicsItem * option,
    QWidget * widget
    )
  {
    // QTextCursor oldCursor = textCursor();
    // QTextCursor newCursor = oldCursor;
    // newCursor.clearSelection();
    // setTextCursor( newCursor );

    // Parent::paint( painter, option, widget );

    // setTextCursor( oldCursor );

    QTextCursor cursor = this->textCursor();
    QTextDocument *document = this->document();
    QAbstractTextDocumentLayout *layout = document->documentLayout();
    QAbstractTextDocumentLayout::PaintContext context;
    QAbstractTextDocumentLayout::Selection selection;
    selection.cursor = cursor;
    context.selections.append( selection );
    layout->draw( painter, context );
  }
};

void TextContainer::buildTextItem()
{
  destroyTextItems();
  if (m_editing)
  {
    EditableTextItem* editableTextItem = new EditableTextItem( this );
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
