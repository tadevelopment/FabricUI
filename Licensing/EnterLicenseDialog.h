/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_LICENSING_ENTERLICENSEDIALOG_H
#define __FABRICUI_LICENSING_ENTERLICENSEDIALOG_H

#include <QCursor>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

#include <FabricUI/Util/LoadFabricStyleSheet.h>

class EnterLicenseDialog : public QDialog
{
  Q_OBJECT

public:

  EnterLicenseDialog( QWidget *parent )
    : QDialog( parent )
    , m_textBox( new QTextEdit( this ) )
  {
    setModal( true );
    setWindowTitle( "Fabric Licensing" );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    setMinimumWidth( 600 );

    QString styleSheet = LoadFabricStyleSheet( "FabricUI.qss" );
    if ( !styleSheet.isEmpty() )
      setStyleSheet( styleSheet );

    setLayout( new QVBoxLayout() );

    QLabel *info = new QLabel( this );
    info->setText( "Copy and paste your node-locked license here:" );
    info->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    layout()->addWidget( info );

    m_textBox->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    layout()->addWidget( m_textBox );

    QPushButton *okButton = new QPushButton( "Ok", this );
    QPushButton *cancelButton = new QPushButton( "Cancel", this );

    QObject::connect( cancelButton, SIGNAL( clicked() ), this,
                      SLOT( reject() ) );
    QObject::connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    layout()->addWidget( okButton );
    layout()->addWidget( cancelButton );

    connect(
      this, SIGNAL(accepted()),
      this, SLOT(onAccepted())
      );
  }

  QString licenseText()
    { return m_textBox->toPlainText(); }

signals:

  void licenseEntered( QString licenseText );

protected slots:
  
  void onAccepted()
  {
    emit licenseEntered( licenseText() );
  }

private:

  QTextEdit *m_textBox;
};

#endif // __FABRICUI_LICENSING_ENTERLICENSEDIALOG_H
