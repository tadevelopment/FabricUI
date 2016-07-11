//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/Dialogs/DFGRegisteredTypeLineEdit.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Elements.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDropEvent>
#include <QtGui/QHeaderView>
#include <QtGui/QMouseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

namespace FabricUI {
namespace DFG {

DFGPEWidget_Elements::DFGPEWidget_Elements(
  DFGWidget *dfgWidget,
  DFGPEModel *model,
  QWidget *parent
  )
  : QWidget( parent )
  , m_dfgWidget( dfgWidget )
  , m_model( NULL )
  , m_ignoreCellChanged( false )
  , m_plusPixmap( LoadPixmap( "DFGPlus.png" ) )
  , m_dotsPixmap( LoadPixmap( "DFGDots.png" ) )
  , m_minusPixmap( LoadPixmap( "DFGMinus.png" ) )
  , m_plusIcon( m_plusPixmap )
  , m_dotsIcon( m_dotsPixmap )
  , m_minusIcon( m_minusPixmap )
  , m_layout( new QVBoxLayout )
{
  setObjectName( "DFGPEWidget_Elements" );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  m_layout->setContentsMargins( 0, 0, 0, 0 );
  m_layout->setSpacing( 0 );
  m_portTypeLabels << "In" << "IO" << "Out";
  setLayout( m_layout );
  setModel( model );
}

void DFGPEWidget_Elements::setModel( DFGPEModel *newModel )
{
  if ( m_model )
  {
    unpopulatePorts();
    m_model->setParent( NULL );
    delete m_model;

    m_layout->removeWidget( m_addPortContainer );
    m_layout->removeWidget( m_tableWidget );
    m_tableWidget->setParent( NULL );
    delete m_tableWidget;
    m_tableWidget = NULL;
  }

  m_model = newModel;

  if ( m_model )
  {
    QString elementDescCapitalized = m_model->getElementDescCapitalized();

    m_isPortTypeFixed = m_model->isPortTypeFixed();
    m_controlCol = 0;
    m_portNameCol = m_controlCol + 1;
    m_portTypeCol = m_isPortTypeFixed? -1: m_portNameCol + 1;
    m_portTypeSpecCol = m_isPortTypeFixed? m_portNameCol + 1: m_portTypeCol + 1;

    m_addPortName = new QLineEdit;
    m_addPortName->setEnabled( !m_model->isReadOnly() );
    if ( !m_isPortTypeFixed )
    {
      m_addPortType = new QComboBox;
      for ( int i = 0; i < m_portTypeLabels.size(); ++i )
        m_addPortType->addItem( m_portTypeLabels[i] );
      m_addPortType->setEnabled( !m_model->isReadOnly() );
    }
    FabricCore::Client client = m_dfgWidget->getDFGController()->getClient();
    m_addPortTypeSpec = new DFGRegisteredTypeLineEdit( NULL, client, "" );
    m_addPortTypeSpec->setEnabled( !m_model->isReadOnly() );
    m_addPortButton =
      new QPushButton( m_plusIcon, "Add " + elementDescCapitalized );
    m_addPortButton->setEnabled( !m_model->isReadOnly() );
    connect(
      m_addPortButton, SIGNAL(clicked()),
      this, SLOT(onAddPortClicked())
      );
    QHBoxLayout *addPortLayout = new QHBoxLayout;
    addPortLayout->setContentsMargins( 0, 0, 0, 0 );
    addPortLayout->addWidget( new QLabel("Name:") );
    addPortLayout->addWidget( m_addPortName );
    if ( !m_isPortTypeFixed )
    {
      addPortLayout->addSpacing( 4 );
      addPortLayout->addWidget( new QLabel("Type:") );
      addPortLayout->addWidget( m_addPortType );
    }
    addPortLayout->addSpacing( 4 );
    addPortLayout->addWidget( new QLabel("TypeSpec:") );
    addPortLayout->addWidget( m_addPortTypeSpec );
    addPortLayout->addSpacing( 4 );
    addPortLayout->addWidget( m_addPortButton );
    addPortLayout->addStretch( 1 );
    m_addPortContainer = new QFrame;
    m_addPortContainer->setObjectName( "DFGPEWidget_Elements_AddPortContainer" );
    m_addPortContainer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    m_addPortContainer->setLayout( addPortLayout );

    QStringList headerLabels;
    headerLabels << "" << (elementDescCapitalized + " Name");
    if ( !m_isPortTypeFixed )
      headerLabels << (elementDescCapitalized + " Type");
    headerLabels << (elementDescCapitalized + " TypeSpec");

    m_tableWidget =
      new DFGPEWidget_Elements_TableWidget( m_model, 0, m_portTypeSpecCol + 1 );
    m_tableWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    if ( !m_isPortTypeFixed )
      m_tableWidget->setItemDelegateForColumn(
        m_portTypeCol,
        new DFGPEWidget_Elements_PortTypeDelegate(
          m_portTypeLabels,
          m_tableWidget
          )
        );
    m_tableWidget->setItemDelegateForColumn(
      m_portTypeSpecCol,
      new DFGPEWidget_Elements_PortTypeSpecDelegate(
        m_dfgWidget->getDFGController()->getClient(),
        m_tableWidget
        )
      );
    m_tableWidget->setHorizontalHeaderLabels( headerLabels );
    m_tableWidget->setEnabled( !m_model->isReadOnly() );
    m_tableWidget->model()->setHeaderData( m_portNameCol, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole );
    if ( !m_isPortTypeFixed )
      m_tableWidget->model()->setHeaderData( m_portTypeCol, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole );
    m_tableWidget->model()->setHeaderData( m_portTypeSpecCol, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole );
    connect(
      m_tableWidget, SIGNAL(cellChanged(int, int)),
      this, SLOT(onCellChanged(int, int))
      );

    m_layout->addWidget( m_addPortContainer );
    m_layout->addWidget( m_tableWidget );

    connect(
      m_model, SIGNAL(isReadOnlyChanged(bool)),
      this, SLOT(onIsReadOnlyChanged(bool))
      );
    connect(
      m_model, SIGNAL(portInserted(int, QString, FabricCore::DFGPortType, QString)),
      this, SLOT(onPortInserted(int, QString, FabricCore::DFGPortType, QString))
      );
    connect(
      m_model, SIGNAL(portRenamed(int, QString)),
      this, SLOT(onPortRenamed(int, QString))
      );
    connect(
      m_model, SIGNAL(portTypeChanged(int, FabricCore::DFGPortType)),
      this, SLOT(onPortTypeChanged(int, FabricCore::DFGPortType))
      );
    connect(
      m_model, SIGNAL(portTypeSpecChanged(int, QString)),
      this, SLOT(onPortTypeSpecChanged(int, QString))
      );
    connect(
      m_model, SIGNAL(portRemoved(int)),
      this, SLOT(onPortRemoved(int))
      );
    connect(
      m_model, SIGNAL(portsReordered(QList<int>)),
      this, SLOT(onPortsReordered(QList<int>))
      );

    m_model->setParent( this );
    populatePorts();
  }
}

void DFGPEWidget_Elements::populatePorts()
{
  int portCount = m_model->getPortCount();
  for ( int portIndex = 0; portIndex < portCount; ++portIndex )
    onPortInserted(
      portIndex,
      m_model->getPortName( portIndex ),
      m_model->getPortType( portIndex ),
      m_model->getPortTypeSpec( portIndex )
      );
}

void DFGPEWidget_Elements::unpopulatePorts()
{
  int portCount = m_model->getPortCount();
  for ( int portIndex = 0; portIndex < portCount; ++portIndex )
    onPortRemoved(
      portCount - portIndex - 1
      );
}

void DFGPEWidget_Elements::onIsReadOnlyChanged(
  bool newIsReadOnly
  )
{
  m_tableWidget->setEnabled( !newIsReadOnly );

  int rowCount = m_tableWidget->rowCount();
  for ( int row = 0; row < rowCount; ++row )
  {
    DFGPEWidget_Elements_ControlCell *controlCell =
      static_cast<DFGPEWidget_Elements_ControlCell *>(
        m_tableWidget->cellWidget( row, m_controlCol )
        );
    controlCell->setEnabled( !m_model->isPortReadOnly( row ) );
  }

  m_addPortName->setEnabled( !newIsReadOnly );
  if ( !m_isPortTypeFixed )
    m_addPortType->setEnabled( !newIsReadOnly );
  m_addPortTypeSpec->setEnabled( !newIsReadOnly );
  m_addPortButton->setEnabled( !newIsReadOnly );
}

void DFGPEWidget_Elements::onPortInserted(
  int index,
  QString name,
  FabricCore::DFGPortType type,
  QString typeSpec
  )
{
  IgnoreCellChangedBracket _(this);

  bool isReadOnly = m_model->isPortReadOnly( index );

  m_tableWidget->insertRow( int(index) );

  QTableWidgetItem *controlTWI = new QTableWidgetItem;
  controlTWI->setFlags(
      Qt::ItemIsSelectable
    // | Qt::ItemIsEditable
    | Qt::ItemIsDragEnabled
    | Qt::ItemIsDropEnabled
    | Qt::ItemIsEnabled
    );
  m_tableWidget->setItem( index, m_controlCol, controlTWI );
  QWidget *controlCellWidget =
    new DFGPEWidget_Elements_ControlCell( index, m_minusIcon, m_dotsIcon );
  controlCellWidget->setEnabled( !m_model->isPortReadOnly( index ) );
  connect(
    controlCellWidget, SIGNAL(oneClicked(int)),
    this, SLOT(onDeleteRowClicked(int))
    );
  connect(
    controlCellWidget, SIGNAL(twoClicked(int)),
    this, SLOT(onInspectRowClicked(int))
    );
  if ( index == 0 )
    m_tableWidget->setColumnWidth(
      m_controlCol,
      controlCellWidget->sizeHint().width()
      );
  m_tableWidget->setCellWidget( int(index), m_controlCol, controlCellWidget );

  QTableWidgetItem *portNameTWI = new QTableWidgetItem( name );
  portNameTWI->setFlags(
      Qt::ItemIsSelectable
    | ( isReadOnly? Qt::ItemFlags(0): Qt::ItemIsEditable )
    | Qt::ItemIsDragEnabled
    | Qt::ItemIsDropEnabled
    | Qt::ItemIsEnabled
    );
  m_tableWidget->setItem( int(index), m_portNameCol, portNameTWI );
  if ( !m_isPortTypeFixed )
  {
    QTableWidgetItem *portTypeTWI = new QTableWidgetItem();
    portTypeTWI->setData( Qt::DisplayRole, m_portTypeLabels[int(type)] );
    portTypeTWI->setFlags(
        Qt::ItemIsSelectable
      | ( isReadOnly? Qt::ItemFlags(0): Qt::ItemIsEditable )
      | Qt::ItemIsDragEnabled
      | Qt::ItemIsDropEnabled
      | Qt::ItemIsEnabled
      );
    m_tableWidget->setItem( int(index), m_portTypeCol, portTypeTWI );
  }
  QTableWidgetItem *portTypeSpecTWI = new QTableWidgetItem( typeSpec );
  portTypeSpecTWI->setFlags(
      Qt::ItemIsSelectable
    | ( isReadOnly? Qt::ItemFlags(0): Qt::ItemIsEditable )
    | Qt::ItemIsDragEnabled
    | Qt::ItemIsDropEnabled
    | Qt::ItemIsEnabled
    );
  m_tableWidget->setItem( int(index), m_portTypeSpecCol, portTypeSpecTWI );

  int rowCount = m_tableWidget->rowCount();
  for ( int row = index + 1; row < rowCount; ++row )
  {
    DFGPEWidget_Elements_ControlCell *controlCell =
      static_cast<DFGPEWidget_Elements_ControlCell *>(
        m_tableWidget->cellWidget( row, m_controlCol )
        );
    controlCell->setRow( row );
  }
}

void DFGPEWidget_Elements::onPortRenamed(
  int index,
  QString newPortName
  )
{
  IgnoreCellChangedBracket _(this);
  QTableWidgetItem *twi = m_tableWidget->item( index, m_portNameCol );
  twi->setText( newPortName );
}

void DFGPEWidget_Elements::onPortTypeChanged(
  int index,
  FabricCore::DFGPortType type
  )
{
  if ( !m_isPortTypeFixed )
  {
    IgnoreCellChangedBracket _(this);
    QTableWidgetItem *twi = m_tableWidget->item( index, m_portTypeCol );
    twi->setData( Qt::DisplayRole, m_portTypeLabels[int(type)] );
  }
}

void DFGPEWidget_Elements::onPortTypeSpecChanged(
  int index,
  QString typeSpec
  )
{
  IgnoreCellChangedBracket _(this);
  QTableWidgetItem *twi = m_tableWidget->item( index, m_portTypeSpecCol );
  twi->setText( typeSpec );
}

void DFGPEWidget_Elements::onPortRemoved(
  int index
  )
{
  IgnoreCellChangedBracket _(this);

  m_tableWidget->removeRow( int(index) );

  int rowCount = m_tableWidget->rowCount();
  for ( int row = int(index); row < rowCount; ++row )
  {
    DFGPEWidget_Elements_ControlCell *controlCell =
      static_cast<DFGPEWidget_Elements_ControlCell *>(
        m_tableWidget->cellWidget( row, m_controlCol )
        );
    controlCell->setRow( row );
  }
}

void DFGPEWidget_Elements::onPortsReordered(
  QList<int> newIndices
  )
{
  // FIXME: should swap in-place to eliminate flickering
  unpopulatePorts();
  populatePorts();
}

void DFGPEWidget_Elements::onCellChanged( int row, int col )
{
  if ( !m_ignoreCellChanged )
  {
    if ( col == m_portNameCol )
    {
      QTableWidgetItem *twi = m_tableWidget->item( row, col );
      m_model->renamePort( row, twi->text() );
    }
    else if ( col == m_portTypeCol )
    {
      QTableWidgetItem *twi = m_tableWidget->item( row, col );
      FabricCore::DFGPortType newPortType;
      QString value = twi->data( Qt::DisplayRole ).toString();
      if ( value == m_portTypeLabels[int(FabricCore::DFGPortType_Out)] )
        newPortType = FabricCore::DFGPortType_Out;
      else if ( value == m_portTypeLabels[int(FabricCore::DFGPortType_IO)] )
        newPortType = FabricCore::DFGPortType_IO;
      else
        newPortType = FabricCore::DFGPortType_In;
      m_model->setPortType( row, newPortType );
    }
    else if ( col == m_portTypeSpecCol )
    {
      QTableWidgetItem *twi = m_tableWidget->item( row, col );
      m_model->setPortTypeSpec( row, twi->text() );
    }
  }
}

void DFGPEWidget_Elements::onInspectRowClicked( int row )
{
  m_model->inspectPort( row, m_dfgWidget );
}

void DFGPEWidget_Elements::onDeleteRowClicked( int row )
{
  m_model->removePort( row );
}

void DFGPEWidget_Elements::onAddPortClicked()
{
  int index = m_model->getPortCount();
  m_model->insertPort(
    index,
    m_addPortName->text(),
    m_isPortTypeFixed?
      FabricCore::DFGPortType_In:
      FabricCore::DFGPortType( m_addPortType->currentIndex() ),
    m_addPortTypeSpec->text()
    );
  m_addPortName->selectAll();
  m_tableWidget->selectRow( index );
}

DFGPEWidget_Elements_ControlCell::DFGPEWidget_Elements_ControlCell(
  int row,
  QIcon iconOne,
  QIcon iconTwo,
  QWidget *parent
  )
  : QFrame( parent )
  , m_row( row )
{
  setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
  setObjectName( "DFGPEWidget_Elements_ControlCell" );

  QPushButton *pushButtonOne = new QPushButton( iconOne, "" );
  connect(
    pushButtonOne, SIGNAL(clicked()),
    this, SLOT(onOneClicked())
    );
  pushButtonOne->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

  QPushButton *pushButtonTwo = new QPushButton( iconTwo, "" );
  pushButtonTwo->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
  connect(
    pushButtonTwo, SIGNAL(clicked()),
    this, SLOT(onTwoClicked())
    );

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( pushButtonOne );
  layout->addWidget( pushButtonTwo );

  setLayout( layout );
}

void DFGPEWidget_Elements_ControlCell::onOneClicked()
{
  emit oneClicked( m_row );
}

void DFGPEWidget_Elements_ControlCell::onTwoClicked()
{
  emit twoClicked( m_row );
}

DFGPEWidget_Elements_TableWidget::DFGPEWidget_Elements_TableWidget(
  DFGPEModel *model,
  int rows,
  int cols,
  QWidget *parent
  )
  : QTableWidget( rows, cols, parent )
  , m_model( model )
{
  setStyle( new DFGPEWidget_Elements_TableWidget_ProxyStyle( style() ) );
  horizontalHeader()->setStretchLastSection( true );
  verticalHeader()->hide();
  // setDragEnabled( True );
  // setAcceptDrops( True );
  // viewport().setAcceptDrops( True );
  setShowGrid( false );
  setDragDropOverwriteMode( false );
  setDropIndicatorShown( true );
  setSelectionMode( QAbstractItemView::ExtendedSelection );
  setSelectionBehavior( QAbstractItemView::SelectRows );
  setDragDropMode( QAbstractItemView::InternalMove );
}

bool DFGPEWidget_Elements_TableWidget::isDragValid( QDropEvent *event )
{
  int rowCount = this->rowCount();

  int targetRow = indexAt( event->pos() ).row();
  if ( targetRow == -1 )
    targetRow = rowCount;

  for ( int row = targetRow; row < rowCount; ++row )
    if ( m_model->isPortReadOnly( row ) )
      return false;

  return true;
}

void DFGPEWidget_Elements_TableWidget::dragEnterEvent( QDragEnterEvent *event )
{
  if ( !isDragValid( event ) )
    return;

  QTableWidget::dragEnterEvent( event );
}

void DFGPEWidget_Elements_TableWidget::dragMoveEvent( QDragMoveEvent *event )
{
  if ( !isDragValid( event ) )
    return;
  
  QTableWidget::dragMoveEvent( event );
}

void DFGPEWidget_Elements_TableWidget::dropEvent( QDropEvent *event )
{
  if ( event->source() != this
    || event->dropAction() != Qt::MoveAction )
  {
    event->ignore();
    return;
  }

  int rowCount = this->rowCount();

  QList<int> selectedRows;
  {
    QList<QTableWidgetItem *> selectedItems = this->selectedItems();
    for ( int i = 0; i < selectedItems.size(); ++i )
    {
      QTableWidgetItem *selectedItem = selectedItems[i];
      if ( selectedItem->column() == 0 )
        selectedRows.push_back( selectedItems[i]->row() );
    }
  }
  qSort( selectedRows );
  if ( selectedRows.isEmpty() )
  {
    event->ignore();
    return;
  }

  int targetRow = indexAt( event->pos() ).row();
  if ( targetRow == -1 )
    targetRow = rowCount;

  QList<int> newIndices;
  for ( int row = 0; row <= rowCount; ++row )
  {
    if ( row == targetRow )
      for ( int i = 0; i < selectedRows.size(); ++i )
        newIndices.push_back( selectedRows[i] );
    if ( row < rowCount
      && !selectedRows.contains( row ) )
      newIndices.push_back( row );
  }

  m_model->reorderPortsEventually( newIndices );

  event->accept();
}

DFGPEWidget_Elements_TableWidget_ProxyStyle::DFGPEWidget_Elements_TableWidget_ProxyStyle(QStyle* style)
  : QProxyStyle( style )
{
}

void DFGPEWidget_Elements_TableWidget_ProxyStyle::drawPrimitive(
    PrimitiveElement element,
    QStyleOption const *option,
    QPainter *painter,
    QWidget const *widget
    ) const
{
  if ( element == QStyle::PE_IndicatorItemViewItemDrop
    && !option->rect.isNull() )
  {
    QStyleOption opt( *option );
    opt.rect.setLeft( 0 );
    opt.rect.setBottom( opt.rect.top() );
    if ( widget )
      opt.rect.setRight( widget->width() );
    QProxyStyle::drawPrimitive(
      element, &opt, painter, widget
      );
    return;
  }
  QProxyStyle::drawPrimitive(
    element, option, painter, widget
    );
}

DFGPEWidget_Elements_PortTypeDelegate::DFGPEWidget_Elements_PortTypeDelegate(
  QStringList portTypeLabels,
  QObject *parent
  )
  : QStyledItemDelegate( parent )
  , m_portTypeLabels( portTypeLabels )
{
}

QWidget *DFGPEWidget_Elements_PortTypeDelegate::createEditor(
  QWidget *parent,
  QStyleOptionViewItem const &option,
  QModelIndex const &index
  ) const
{
  DFGPEWidget_Elements_PortTypeDelegate_ComboxBox *comboBox =
    new DFGPEWidget_Elements_PortTypeDelegate_ComboxBox( parent );
  for ( int i = 0; i < m_portTypeLabels.size(); ++i )
    comboBox->addItem( m_portTypeLabels[i] );
  return comboBox;
}

void DFGPEWidget_Elements_PortTypeDelegate::setEditorData(
  QWidget *editor,
  QModelIndex const &index
  ) const
{
  QComboBox *comboBox = static_cast<QComboBox*>( editor );
  QString value = index.model()->data( index, Qt::DisplayRole ).toString();
  for ( int i = 0; i < m_portTypeLabels.size(); ++i )
    if ( value == m_portTypeLabels[i] )
    {
      comboBox->setCurrentIndex( i );
      break;
    }
  // See http://www.qtforum.org/article/32504/using-qcombobox-for-item-delegate.html
  QApplication::instance()->postEvent(
    comboBox,
    new QMouseEvent(
      QEvent::MouseButtonPress,
      QPoint(0, 0),
      Qt::LeftButton,
      Qt::LeftButton,
      Qt::NoModifier
      )
    );
}

void DFGPEWidget_Elements_PortTypeDelegate::setModelData(
  QWidget *editor,
  QAbstractItemModel *model,
  QModelIndex const &index
  ) const
{
  QComboBox *comboBox = static_cast<QComboBox*>( editor );
  model->setData(
    index,
    m_portTypeLabels[comboBox->currentIndex()],
    Qt::DisplayRole
    );
}

void DFGPEWidget_Elements_PortTypeDelegate::updateEditorGeometry(
  QWidget *editor,
  QStyleOptionViewItem const &option,
  QModelIndex const &index
  ) const
{
  editor->setGeometry( option.rect );
}

DFGPEWidget_Elements_PortTypeDelegate_ComboxBox::DFGPEWidget_Elements_PortTypeDelegate_ComboxBox(
  QWidget *parent
  )
  : QComboBox( parent )
{
  connect(
    this, SIGNAL(activated(int)),
    this, SLOT(onActivated(int))
    );
}

void DFGPEWidget_Elements_PortTypeDelegate_ComboxBox::onActivated(
  int index
  )
{
  qDebug() << index;
  QApplication::instance()->postEvent(
    this,
    new QKeyEvent(
      QEvent::KeyPress,
      Qt::Key_Return,
      Qt::NoModifier
      )
    );
  QApplication::instance()->postEvent(
    this,
    new QKeyEvent(
      QEvent::KeyRelease,
      Qt::Key_Return,
      Qt::NoModifier
      )
    );
}

DFGPEWidget_Elements_PortTypeSpecDelegate::DFGPEWidget_Elements_PortTypeSpecDelegate(
  FabricCore::Client client,
  QObject *parent
  )
  : QStyledItemDelegate( parent )
  , m_client( client )
{
}

QWidget *DFGPEWidget_Elements_PortTypeSpecDelegate::createEditor(
  QWidget *parent,
  QStyleOptionViewItem const &option,
  QModelIndex const &index
  ) const
{
  DFGRegisteredTypeLineEdit *lineEdit =
    new DFGRegisteredTypeLineEdit(
      parent,
      const_cast<DFGPEWidget_Elements_PortTypeSpecDelegate *>( this )->m_client,
      ""
      );
  return lineEdit;
}

void DFGPEWidget_Elements_PortTypeSpecDelegate::setEditorData(
  QWidget *editor,
  QModelIndex const &index
  ) const
{
  DFGRegisteredTypeLineEdit *lineEdit =
    static_cast<DFGRegisteredTypeLineEdit*>( editor );
  QString value = index.model()->data( index, Qt::DisplayRole ).toString();
  lineEdit->setText( value );
}

void DFGPEWidget_Elements_PortTypeSpecDelegate::setModelData(
  QWidget *editor,
  QAbstractItemModel *model,
  QModelIndex const &index
  ) const
{
  DFGRegisteredTypeLineEdit *lineEdit =
    static_cast<DFGRegisteredTypeLineEdit*>( editor );
  QString value = lineEdit->text();
  model->setData( index, value, Qt::DisplayRole );
}

void DFGPEWidget_Elements_PortTypeSpecDelegate::updateEditorGeometry(
  QWidget *editor,
  QStyleOptionViewItem const &option,
  QModelIndex const &index
  ) const
{
  editor->setGeometry( option.rect );
}

} // namespace DFG
} // namespace FabricUI
