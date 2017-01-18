// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_TextContainer__
#define __UI_GraphView_TextContainer__

#include <QGraphicsWidget>
#include <QGraphicsTextItem>
#include <QColor>
#include <QFont>
#include <QTimer>

namespace FabricUI
{

  namespace GraphView
  {
    class TextContainer_EditableTextItem;

    class TextContainer : public QGraphicsWidget
    {
      friend class TextContainer_EditableTextItem;

    public:

      TextContainer(
        QGraphicsWidget * parent,
        QString const &text,
        QColor color,
        QColor hlColor,
        QFont font,
        bool editable = false
        );
      virtual ~TextContainer();

      // might differ from the displayed text while editing
      virtual QString text() const
        { return m_text; }
      virtual void setText(QString const &text);
      void setSuffix(QString const& suffix);
      virtual QColor color() const;
      virtual QColor highlightColor() const;
      virtual void setColor(QColor color, QColor hlColor);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      virtual QFont font() const;
      virtual void setFont(QFont font);
      virtual bool italic() const;
      virtual void setItalic(bool flag);
      virtual void setEditable(bool editable) {
        m_editable = editable;
        if( !editable ) { setEditing( false ); }
      }

    protected:

      void refresh();
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
        if( m_editable) { setEditing( true ); }
        QGraphicsWidget::mouseDoubleClickEvent(event);
      }
      virtual void submitEditedText(const QString& text) {} // to override

      // called when the text displayed changes (even if it's not submitted)
      // TODO: use signals/slots instead ?
      virtual void displayedTextChanged();

    private:

      QColor m_color;
      QFont m_font;
      QColor m_highlightColor;
      QString m_text;
      // the suffix won't be displayed while editing
      QString m_suffix;
      bool m_highlighted;

      bool m_editable;

      bool m_editing;
      QGraphicsSimpleTextItem * m_fixedTextItem;
      class EditableTextItem;
      QGraphicsTextItem * m_editableTextItem;

      void setEditing(bool editable);
      void buildTextItem();
      void destroyTextItems();
    };


    class TextContainer_EditableTextItem
      : public QGraphicsTextItem
    {

      Q_OBJECT

      typedef QGraphicsTextItem Parent;

      TextContainer* m_container;
      QTimer *m_timer;
      bool m_displayCursor;

    public:

      TextContainer_EditableTextItem( TextContainer* container );

      void selectAllText();

    private:

      void exit(bool submit);

    protected:

      void focusOutEvent(QFocusEvent *event);

      void keyPressEvent(QKeyEvent* event);

      virtual void paint(
        QPainter * painter,
        const QStyleOptionGraphicsItem * option,
        QWidget * widget
        );

    public slots:

      void cursorFlash();
    };

  };

};

#endif // __UI_GraphView_TextContainer__
