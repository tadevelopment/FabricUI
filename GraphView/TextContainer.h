// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_TextContainer__
#define __UI_GraphView_TextContainer__

#include <QGraphicsWidget>
#include <QGraphicsTextItem>
#include <QColor>
#include <QFont>

namespace FabricUI
{

  namespace GraphView
  {

    class TextContainer : public QGraphicsWidget
    {
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
      virtual QColor color() const;
      virtual QColor highlightColor() const;
      virtual void setColor(QColor color, QColor hlColor);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      virtual QFont font() const;
      virtual void setFont(QFont font);
      virtual bool italic() const;
      virtual void setItalic(bool flag);
      void setEditable(bool editable) {
        m_editable = editable;
        if( !editable ) { setEditing( false ); }
      }

    protected:

      void refresh();
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
        if( m_editable) { setEditing( true ); }
        QGraphicsWidget::mouseDoubleClickEvent(event);
      }
      virtual void submitEditedText(const QString& text) {
        // to override
      }

    private:

      QColor m_color;
      QFont m_font;
      QColor m_highlightColor;
      QString m_text;
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

  };

};

#endif // __UI_GraphView_TextContainer__
