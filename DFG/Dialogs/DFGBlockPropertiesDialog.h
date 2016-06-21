// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGBlockPropertiesDialog__
#define __UI_DFG_DFGBlockPropertiesDialog__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QShowEvent>
#include <QtGui/QCheckBox>
#include "DFGBaseDialog.h"
#include "DFGRegisteredTypeLineEdit.h"
#include "DFGExtensionLineEdit.h"
#include <FabricUI/ValueEditor_Legacy/ColorValueWidget.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGBlockPropertiesDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:
      /// Constructor
      DFGBlockPropertiesDialog(
        QWidget * parent, 
        DFGController * controller, 
        const char * nodeName, 
        const DFGConfig & dfgConfig = DFGConfig(),
        bool setAlphaNum = false
        );
      /// Destructor
      virtual ~DFGBlockPropertiesDialog();

      /// Shows this dialog widgets
      virtual void showEvent(QShowEvent * event);

      // [Julien] 
      /// Allows only alpha-numeric text (here the title) only 
      void alphaNumicStringOnly();
      // [Julien] 
      /// Filter the QLineEdit text (here the title) with the setRegexFilter
      void setRegexFilter(QString regex);

      QString getText() const
        { return m_textEdit->text(); }
      /// Gets the user selected node's script name
      QString getScriptName();
      /// Gets the user selected node's body color 
      QColor getNodeColor();
      /// Gets the user selected node's header color 
      bool getHeaderColor(QColor &color);
      /// Gets the user selected node's text color 
      QColor getTextColor();

    protected slots:

      void onNodeColorButtonClicked();
      void onHeaderColorButtonClicked();
      void onTextColorButtonClicked();

      // [Julien] FE-5246 : Creates the node header color property
      // Custom header colors can have contrast mistmatches with the body's color
      // Thus, the option is disable by default 
      /// Creates the node header color property
      void onAllowHeaderColorCheckBoxClicked();

    private:

      class ColorButton : public QPushButton
      {
      public:

        ColorButton( QColor const &color, QWidget *parent )
          : QPushButton( parent )
          , m_color( color )
          {}

        QColor const &color() const
          { return m_color; }

        void setColor( QColor const &color )
        {
          m_color = color;
          update();
        }

      protected:
        
        virtual void paintEvent( QPaintEvent *event );

      private:

        QColor m_color;
      };

      /// \internal
      /// Gets the color property header color metadata
      QColor getColorFromBlock( FTL::CStrRef key, QColor const &defaultCol );

      std::string                      m_nodeName;
      DFGController                   *m_controller;

      QColor                           m_nodeDefaultHeaderColor;
      QLineEdit *m_textEdit;
      QLineEdit                       *m_nameEdit;
      ColorButton *m_nodeColorButton;
      ColorButton *m_headerColorButton;
      QCheckBox *m_allowHeaderColorCheckBox;
      ColorButton *m_textColorButton;
    };

  };

};

#endif // __UI_DFG_DFGBlockPropertiesDialog__
