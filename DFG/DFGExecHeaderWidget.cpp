// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtWidgets/QProxyStyle>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGExecHeaderWidget::DFGExecHeaderWidget(
  QWidget * parent,
  DFGController *dfgController,
  const GraphView::GraphConfig &config
  )
  : QFrame( parent )
  , m_dfgController( dfgController )
{
  setObjectName( "DFGExecHeaderWidget" );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
  setContentsMargins(0, 0, 0, 0);

  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  m_backgroundColor = config.headerBackgroundColor;
  m_pen = config.headerPen;

  QIcon backIcon = LoadPixmap( "DFGBack.png" );
  m_backButton = new QPushButton( backIcon, "Back" );
  m_backButton->setObjectName( "DFGBackButton" );
  m_backButton->setFocusPolicy( Qt::NoFocus );
  m_backButton->setAutoFillBackground(false);
  QObject::connect(
    m_backButton, SIGNAL(clicked()),
    this, SIGNAL(goUpPressed())
    );

  m_execPathLabel = new QLabel;
  m_execPathLabel->setObjectName( "DFGExecPathLabel" );

  m_presetNameLabel = new QLabel;
  m_presetNameLabel->setObjectName( "DFGPresetNameLabel" );

  m_presetPathSep = new QLabel;
  m_presetPathSep->setObjectName( "DFGPresetPathSep" );

  m_reloadButton = new QPushButton( "Reload" );
  m_reloadButton->setObjectName( "DFGReloadButton" );
  m_reloadButton->setFocusPolicy( Qt::NoFocus );
  m_reloadButton->setAutoFillBackground(false);
  connect(
    m_reloadButton, SIGNAL(clicked()),
    this, SIGNAL(reloadPressed())
    );

  m_saveButton = new QPushButton( "Save" );
  m_saveButton->setObjectName( "DFGSaveButton" );
  m_saveButton->setFocusPolicy( Qt::NoFocus );
  m_saveButton->setAutoFillBackground(false);
  connect(
    m_saveButton, SIGNAL(clicked()),
    this, SIGNAL(savePressed())
    );

  m_reqExtLabel = new QLabel;
  m_reqExtLabel->setObjectName( "DFGRequiredExtensionsLabel" );
  m_reqExtLineEdit = new QLineEdit;
  m_reqExtLineEdit->setObjectName( "DFGRequiredExtensionsLineEdit" );
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(editingFinished()),
    this, SLOT(reqExtEditingFinished())
    );

  layout->addWidget( m_presetNameLabel );
  layout->addWidget( m_presetPathSep );
  layout->addWidget( m_execPathLabel );
  layout->addStretch( 1 );  
  layout->addWidget( m_backButton );
  layout->addWidget( m_reloadButton );
  layout->addWidget( m_saveButton );
  layout->addWidget( m_reqExtLabel );
  layout->addWidget( m_reqExtLineEdit );

  QFrame *regWidget = new QFrame;
  regWidget->setObjectName( "DFGRegWidget" );
  regWidget->setLayout( layout );

  QPushButton *presetSplitButton = new QPushButton( "Split from Preset" );
  presetSplitButton->setObjectName( "DFGPresetSplitButton" );
  presetSplitButton->setSizePolicy(
    QSizePolicy::Minimum, QSizePolicy::Minimum
    );
  connect(
    presetSplitButton, SIGNAL(clicked()),
    this, SLOT(onSplitFromPresetClicked())
    );

  QLabel *presetSplitLabel = new QLabel;
  presetSplitLabel->setObjectName( "DFGPresetSplitLabel" );
  presetSplitLabel->setSizePolicy(
    QSizePolicy::Expanding, QSizePolicy::Minimum
    );

  QHBoxLayout *presetSplitLayout = new QHBoxLayout;
  presetSplitLayout->setContentsMargins( 0, 0, 0, 0 );
  presetSplitLayout->addWidget( presetSplitLabel );
  presetSplitLayout->addWidget( presetSplitButton );

  m_presetSplitWidget = new QFrame;
  m_presetSplitWidget->setObjectName( "DFGPresetSplitWidget" );
  m_presetSplitWidget->setLayout( presetSplitLayout );

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->setContentsMargins( 0, 0, 0, 0 );
  vLayout->setSpacing( 0 );
  vLayout->addWidget( m_presetSplitWidget );
  vLayout->addWidget( regWidget );
  setLayout( vLayout );

  QObject::connect(
    m_dfgController, SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
  QObject::connect(
    m_dfgController, SIGNAL(execSplitChanged()),
    this, SLOT(onExecChanged())
    );
  onExecChanged();
}

DFGExecHeaderWidget::~DFGExecHeaderWidget()
{
}

void DFGExecHeaderWidget::refresh()
{
  FTL::CStrRef execPath = getExecPath();
  FabricCore::DFGExec &exec = getExec();
  FTL::StrRef execBlockName = m_dfgController->getExecBlockName();
  if ( exec )
  {
    bool isRoot = execPath.empty();
    bool isPreset = exec.isPreset();
    bool wouldSplitFromPreset = exec.editWouldSplitFromPreset();

    m_presetSplitWidget->setVisible( wouldSplitFromPreset );

    m_backButton->setVisible( !isRoot );

    m_execPathLabel->setVisible( !isRoot );
    if ( !isRoot )
    {
      QString pathLabelText( "Path: " );
      pathLabelText += QString::fromUtf8( execPath.data(), execPath.size() );
      if ( !execBlockName.empty() )
      {
        if ( !execPath.empty() )
          pathLabelText += '.';
        pathLabelText += QString::fromUtf8( execBlockName.data(), execBlockName.size() );
      }
      m_execPathLabel->setText( pathLabelText );
    }

    m_presetNameLabel->setVisible( isPreset );
    if ( isPreset )
    {
      FTL::CStrRef title = exec.getTitle();
      QString presetNameText( "Preset Name: ");
      presetNameText += QString::fromUtf8( title.data(), title.size() );
      m_presetNameLabel->setText( presetNameText );
    }
    m_presetPathSep->setVisible( isPreset );

    bool showFuncButtons = execBlockName.empty()
      && exec.getType() == FabricCore::DFGExecType_Func;
    m_reloadButton->setVisible( showFuncButtons );
    m_saveButton->setVisible( showFuncButtons );
    m_reloadButton->setEnabled( !isPreset );
    m_saveButton->setEnabled( !isPreset );

    FabricCore::String extDepsDesc = exec.getExtDeps();
    FTL::CStrRef extDepsDescCStr =
      extDepsDesc.getCStr()? extDepsDesc.getCStr() : "";
    QString reqExtLabelText( "Required Extensions:" );
    m_reqExtLineEdit->setVisible(
      execBlockName.empty() && !wouldSplitFromPreset
      );
    if ( wouldSplitFromPreset )
    {
      reqExtLabelText += ' ';
      QString reqExts = QString::fromUtf8( extDepsDescCStr.data(), extDepsDescCStr.size() );
      if ( !reqExts.isEmpty() )
        reqExtLabelText += reqExts;
      else
        reqExtLabelText += "(none)";
    }
    else m_reqExtLineEdit->setText( extDepsDescCStr.c_str() );
    m_reqExtLabel->setVisible( execBlockName.empty() );
    m_reqExtLabel->setText( reqExtLabelText );

    update();
  }
}

void DFGExecHeaderWidget::refreshTitle( FTL::CStrRef title )
{
  refresh();
}

void DFGExecHeaderWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  refresh();
}

bool DFGExecHeaderWidget::reqExtLineEditWidgetHasFocus() const
{
  return (m_reqExtLineEdit && QApplication::focusWidget() == m_reqExtLineEdit);
}

bool DFGExecHeaderWidget::reqExtLineEditWidgetClearFocus()
{
  if (!m_reqExtLineEdit)
    return false;

  // cancel any text changes.
  FabricCore::String currentExtDepDesc = getExec().getExtDeps();
  m_reqExtLineEdit->setText(currentExtDepDesc.getCStr());

  // remove keyboard focus.
  m_reqExtLineEdit->clearFocus();

  return true;
}

void DFGExecHeaderWidget::reqExtEditingFinished()
{
  std::string extDepDesc = m_reqExtLineEdit->text().toUtf8().constData();  
  FabricCore::String currentExtDepDesc = getExec().getExtDeps();
  char const *currentExtDepDescCStr = currentExtDepDesc.getCStr();
  if ( !currentExtDepDescCStr )
    currentExtDepDescCStr = "";
  if ( extDepDesc == currentExtDepDescCStr )
    return;

  FTL::StrRef::Split split = FTL::StrRef( extDepDesc ).split(',');
  QStringList nameAndVers;
  while ( !split.first.empty() )
  {
    FTL::StrRef trimmed = split.first.trim();
    nameAndVers.append(
      QString::fromUtf8( trimmed.data(), trimmed.size() )
      );
    split = split.second.split(',');
  }

  m_dfgController->cmdSetExtDeps( nameAndVers );
}

void DFGExecHeaderWidget::onExecChanged()
{
  refresh();
}

void DFGExecHeaderWidget::paintEvent(QPaintEvent * event)
{
  QRect rect = contentsRect();
  QPainter painter(this);

  painter.fillRect(rect, m_backgroundColor);

  painter.setPen(m_pen);
  painter.drawLine(rect.bottomLeft(), rect.bottomRight());

  QWidget::paintEvent(event);
}

FTL::CStrRef DFGExecHeaderWidget::getExecPath()
{
  return m_dfgController->getExecPath();
}

FabricCore::DFGExec &DFGExecHeaderWidget::getExec()
{
  return m_dfgController->getExec();
}

void DFGExecHeaderWidget::onSplitFromPresetClicked()
{
  m_dfgController->cmdSplitFromPreset();
}
