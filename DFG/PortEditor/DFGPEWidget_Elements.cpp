//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/Dialogs/DFGRegisteredTypeLineEdit.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Elements.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <QDebug>
#include <QApplication>
#include <QComboBox>
#include <QDropEvent>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

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
  , m_editPixmap( LoadPixmap( "DFGEdit.png" ) )
  , m_minusPixmap( LoadPixmap( "DFGMinus.png" ) )
  , m_plusIcon( m_plusPixmap )
  , m_editIcon( m_editPixmap )
  , m_minusIcon( m_minusPixmap )
  , m_portNameRegExp( "^[a-zA-Z][_a-zA-Z0-9]*$" )
  , m_typeSpecRegExp( "^[^ ]*$" )
  , m_layout( new QVBoxLayout )
{
  setObjectName( "DFGPEWidget_Elements" );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
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

    m_layout->removeWidget( m_addElementContainer );
    m_layout->removeWidget( m_tableWidget );
    m_tableWidget->setParent( NULL );
    delete m_tableWidget;
    m_tableWidget = NULL;
  }

  m_model = newModel;

  if ( m_model )
  {
    QString elementDescCapitalized = m_model->getElementDescCapitalized();

    m_canInspectElements = m_model->canInspectElements();
    m_hasPortType = m_model->hasPortType();
    m_hasTypeSpec = m_model->hasTypeSpec();
    int curCol = 0;
    m_controlCol = curCol++;
    m_portNameCol = curCol++;
    m_portTypeCol = m_hasPortType? curCol++: -1;
    m_portTypeSpecCol = m_hasTypeSpec? curCol++: -1;
    int colCount = curCol;

    m_addElementName = new QLineEdit;
    m_addElementName->setEnabled( !m_model->isReadOnly() );
    m_addElementName->setValidator(
      new QRegExpValidator( m_portNameRegExp )
      );
    if ( m_hasPortType )
    {
      m_addElementType = new QComboBox;
      for ( int i = 0; i < m_portTypeLabels.size(); ++i )
        m_addElementType->addItem( m_portTypeLabels[i] );
      m_addElementType->setEnabled( !m_model->isReadOnly() );
    }
    if ( m_hasTypeSpec )
    {
      FabricCore::Client client = m_dfgWidget->getDFGController()->getClient();
      m_addElementTypeSpec = new DFGRegisteredTypeLineEdit( this, client, "" );
      m_addElementTypeSpec->setEnabled( !m_model->isReadOnly() );
      m_addElementTypeSpec->setValidator(
        new QRegExpValidator( m_typeSpecRegExp )
        );
      connect(m_dfgWidget->getHeaderWidget(), SIGNAL(extensionLoaded()),
        m_addElementTypeSpec, SLOT(onUpdateRegisteredTypeList()));
    }
    m_addElementButton =
      new QPushButton( m_plusIcon, "Add " + elementDescCapitalized );
    m_addElementButton->setEnabled( !m_model->isReadOnly() );
    m_addElementButton->setDefault(true);
    connect(
      m_addElementButton, SIGNAL(clicked()),
      this, SLOT(onAddElementClicked())
      );
    QHBoxLayout *addElementLayout = new QHBoxLayout;
    addElementLayout->setContentsMargins( 0, 0, 0, 0 );
    addElementLayout->addWidget( new QLabel("Name:") );
    addElementLayout->addWidget( m_addElementName );
    if ( m_hasPortType )
    {
      addElementLayout->addSpacing( 4 );
      addElementLayout->addWidget( new QLabel("Type:") );
      addElementLayout->addWidget( m_addElementType );
    }
    if ( m_hasTypeSpec )
    {
      addElementLayout->addSpacing( 4 );
      addElementLayout->addWidget( new QLabel("TypeSpec:") );
      addElementLayout->addWidget( m_addElementTypeSpec );
    }
    addElementLayout->addSpacing( 4 );
    addElementLayout->addWidget( m_addElementButton );
    addElementLayout->addStretch( 1 );
    m_addElementContainer = new QFrame;
    m_addElementContainer->setObjectName( "DFGPEWidget_Elements_AddElementContainer" );
    m_addElementContainer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    m_addElementContainer->setLayout( addElementLayout );

    QStringList headerLabels;
    headerLabels << "" << (elementDescCapitalized + " Name");
    if ( m_hasPortType )
      headerLabels << (elementDescCapitalized + " Type");
    if ( m_hasTypeSpec )
      headerLabels << (elementDescCapitalized + " TypeSpec");

    m_tableWidget =
      new DFGPEWidget_Elements_TableWidget( m_model, 0, colCount );
    m_tableWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_tableWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect(
      m_tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
      this, SLOT(onCustomContextMenuRequested(const QPoint &))
      );
    m_tableWidget->setItemDelegateForColumn(
      m_portNameCol,
      new DFGPEWidget_Elements_PortNameDelegate( m_portNameRegExp )
      );
    if ( m_hasPortType )
      m_tableWidget->setItemDelegateForColumn(
        m_portTypeCol,
        new DFGPEWidget_Elements_PortTypeDelegate(
          m_portTypeLabels,
          m_tableWidget
          )
        );
    if ( m_hasTypeSpec )
      m_tableWidget->setItemDelegateForColumn(
        m_portTypeSpecCol,
        new DFGPEWidget_Elements_PortTypeSpecDelegate(
          m_dfgWidget->getDFGController()->getClient(),
          m_typeSpecRegExp,
          m_tableWidget
          )
        );
    m_tableWidget->setHorizontalHeaderLabels( headerLabels );
    m_tableWidget->model()->setHeaderData( m_portNameCol, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole );
    if ( m_hasPortType )
      m_tableWidget->model()->setHeaderData( m_portTypeCol, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole );
    if ( m_hasTypeSpec )
      m_tableWidget->model()->setHeaderData( m_portTypeSpecCol, Qt::Horizontal, Qt::AlignLeft, Qt::TextAlignmentRole );
    connect(
      m_tableWidget, SIGNAL(cellChanged(int, int)),
      this, SLOT(onCellChanged(int, int))
      );

    m_layout->addWidget( m_addElementContainer );
    m_layout->addWidget( m_tableWidget );

    connect(
      m_model, SIGNAL(isReadOnlyChanged(bool)),
      this, SLOT(onIsReadOnlyChanged(bool))
      );
    connect(
      m_model, SIGNAL(elementInserted(int, QString, FabricCore::DFGPortType, QString)),
      this, SLOT(onPortInserted(int, QString, FabricCore::DFGPortType, QString))
      );
    connect(
      m_model, SIGNAL(elementRenamed(int, QString)),
      this, SLOT(onPortRenamed(int, QString))
      );
    connect(
      m_model, SIGNAL(elementPortTypeChanged(int, FabricCore::DFGPortType)),
      this, SLOT(onPortTypeChanged(int, FabricCore::DFGPortType))
      );
    connect(
      m_model, SIGNAL(elementTypeSpecChanged(int, QString)),
      this, SLOT(onPortTypeSpecChanged(int, QString))
      );
    connect(
      m_model, SIGNAL(elementRemoved(int)),
      this, SLOT(onPortRemoved(int))
      );
    connect(
      m_model, SIGNAL(elementsReordered(QList<int>)),
      this, SLOT(onPortsReordered(QList<int>))
      );

    m_model->setParent( this );
    populatePorts();
  }
}

void DFGPEWidget_Elements::populatePorts()
{
  int portCount = m_model->getElementCount();
  for ( int portIndex = 0; portIndex < portCount; ++portIndex )
    onPortInserted(
      portIndex,
      m_model->getElementName( portIndex ),
      m_model->getElementPortType( portIndex ),
      m_model->getElementTypeSpec( portIndex )
      );
}

void DFGPEWidget_Elements::unpopulatePorts()
{
  int portCount = m_model->getElementCount();
  for ( int portIndex = 0; portIndex < portCount; ++portIndex )
    onPortRemoved(
      portCount - portIndex - 1
      );
}

void DFGPEWidget_Elements::onInspectSelected()
{
  QList<QTableWidgetItem *> selectedItems = m_tableWidget->selectedItems();
  QList<int> selectedIndices;
  for ( int i = 0; i < selectedItems.size(); ++i )
    if ( selectedItems[i]->column() == 0 )
      selectedIndices << selectedItems[0]->row();
  assert( selectedIndices.size() == 1 );
  m_model->inspectElement( selectedIndices[0], m_dfgWidget );
}

void DFGPEWidget_Elements::onRemoveSelected()
{
  QList<QTableWidgetItem *> selectedItems = m_tableWidget->selectedItems();
  QList<int> selectedIndices;
  for ( int i = 0; i < selectedItems.size(); ++i )
    if ( selectedItems[i]->column() == 0 )
    {
      int index = selectedItems[i]->row();
      if ( !m_model->isElementReadOnly( index ) )
        selectedIndices << index;
    }
  m_model->removeElements( selectedIndices );
}

void DFGPEWidget_Elements::onCustomContextMenuRequested( QPoint const &pos )
{
  QMenu menu( this );

  QList<QTableWidgetItem *> selectedItems = m_tableWidget->selectedItems();
  QList<int> selectedIndices;
  for ( int i = 0; i < selectedItems.size(); ++i )
    if ( selectedItems[i]->column() == 0 )
      selectedIndices << selectedItems[0]->row();

  if ( m_canInspectElements )
  {
    QAction *inspectAction =
      new QAction( m_editIcon, "Edit Selected", &menu );
    connect(
      inspectAction, SIGNAL(triggered()),
      this, SLOT(onInspectSelected())
      );
    menu.addAction( inspectAction );
    inspectAction->setEnabled( selectedIndices.size() == 1 );
  }

  QAction *removeAction =
    new QAction( m_minusIcon, "Remove Selected", &menu );
  connect(
    removeAction, SIGNAL(triggered()),
    this, SLOT(onRemoveSelected())
    );
  menu.addAction( removeAction );
  bool canRemoveAtLeastOne = false;
  for ( int i = 0; i < selectedIndices.size(); ++i )
    if ( !m_model->isElementReadOnly( selectedIndices[i] ) )
    {
      canRemoveAtLeastOne = true;
      break;
    }
  removeAction->setEnabled( canRemoveAtLeastOne );

  menu.exec( m_tableWidget->mapToGlobal( pos ) );
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
    controlCell->setEnabled( !m_model->isElementReadOnly( row ) );
  }

  m_addElementName->setEnabled( !newIsReadOnly );
  if ( m_hasPortType )
    m_addElementType->setEnabled( !newIsReadOnly );
  if ( m_hasTypeSpec )
    m_addElementTypeSpec->setEnabled( !newIsReadOnly );
  m_addElementButton->setEnabled( !newIsReadOnly );
}

void DFGPEWidget_Elements::onPortInserted(
  int index,
  QString name,
  FabricCore::DFGPortType type,
  QString typeSpec
  )
{
  IgnoreCellChangedBracket _(this);

  bool isReadOnly = m_model->isElementReadOnly( index );

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
  QWidget *controlCellWidget;
  if ( m_canInspectElements )
    controlCellWidget =
      new DFGPEWidget_Elements_ControlCell( index, m_minusIcon, m_editIcon );
  else
    controlCellWidget =
      new DFGPEWidget_Elements_ControlCell( index, m_minusIcon );
  controlCellWidget->setEnabled( !m_model->isElementReadOnly( index ) );
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
  if ( m_hasPortType )
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
  if ( m_hasTypeSpec )
  {
    QTableWidgetItem *portTypeSpecTWI = new QTableWidgetItem( typeSpec );
    portTypeSpecTWI->setFlags(
        Qt::ItemIsSelectable
      | ( isReadOnly? Qt::ItemFlags(0): Qt::ItemIsEditable )
      | Qt::ItemIsDragEnabled
      | Qt::ItemIsDropEnabled
      | Qt::ItemIsEnabled
      );
    m_tableWidget->setItem( int(index), m_portTypeSpecCol, portTypeSpecTWI );
  }

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
  if ( m_hasPortType )
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
  if ( m_hasTypeSpec )
  {
    IgnoreCellChangedBracket _(this);
    QTableWidgetItem *twi = m_tableWidget->item( index, m_portTypeSpecCol );
    twi->setText( typeSpec );
  }
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
      m_model->renameElement( row, twi->text() );
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
      m_model->setElementPortType( row, newPortType );
    }
    else if ( col == m_portTypeSpecCol )
    {
      QTableWidgetItem *twi = m_tableWidget->item( row, col );
      m_model->setElementTypeSpec( row, twi->text() );
    }
  }
}

void DFGPEWidget_Elements::onInspectRowClicked( int row )
{
  m_model->inspectElement( row, m_dfgWidget );
}

void DFGPEWidget_Elements::onDeleteRowClicked( int row )
{
  QList<int> indices;
  indices << row;
  m_model->removeElements( indices );
}

void DFGPEWidget_Elements::onAddElementClicked()
{
  // FE-7961 : Check that the port dataType is valid
  if(m_hasTypeSpec && !m_addElementTypeSpec->checkIfTypeExist())
  {
    m_addElementTypeSpec->displayInvalidTypeWarning();
    return;
  }

  int index = m_model->getElementCount();
  m_model->insertElement(
    index,
    m_addElementName->text(),
    m_hasPortType?
      FabricCore::DFGPortType( m_addElementType->currentIndex() ):
      FabricCore::DFGPortType_In,
    m_hasTypeSpec?
      m_addElementTypeSpec->text():
      QString()
    );
  m_addElementName->selectAll();
  m_tableWidget->selectRow( index );
  emit elementAddedThroughUI( index );
  m_addElementName->setFocus(); // [FE-7054]
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

  QPushButton *pushButtonTwo;
  if ( !iconTwo.isNull() )
  {
    pushButtonTwo = new QPushButton( iconTwo, "" );
    pushButtonTwo->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    connect(
      pushButtonTwo, SIGNAL(clicked()),
      this, SLOT(onTwoClicked())
      );
  }
  else pushButtonTwo = NULL;

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( pushButtonOne );
  if ( pushButtonTwo )
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
  if ( model->allowReordering() )
  {
    setDragDropMode( QAbstractItemView::InternalMove );
    setDragDropOverwriteMode( false );
    setDropIndicatorShown( true );
  }
  else setDragDropMode( QAbstractItemView::NoDragDrop );
  setSelectionMode( QAbstractItemView::ExtendedSelection );
  setSelectionBehavior( QAbstractItemView::SelectRows );
}

bool DFGPEWidget_Elements_TableWidget::isDragValid(
  QDropEvent *event,
  QList<int> &newIndices
  )
{
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

  int targetRow = indexAt( event->pos() ).row();
  if ( targetRow == -1 )
    targetRow = rowCount;

  for ( int row = 0; row <= rowCount; ++row )
  {
    if ( row == targetRow )
      for ( int i = 0; i < selectedRows.size(); ++i )
        newIndices.push_back( selectedRows[i] );
    if ( row < rowCount
      && !selectedRows.contains( row ) )
      newIndices.push_back( row );
  }

  bool someChanged = false;
  for ( int row = 0; row < rowCount; ++row )
    if ( newIndices[row] != row )
    {
      someChanged = true;
      if ( m_model->isElementReadOnly( row ) )
        return false;
    }

  return someChanged;
}

void DFGPEWidget_Elements_TableWidget::dragEnterEvent( QDragEnterEvent *event )
{
  QTableWidget::dragEnterEvent( event );
  QList<int> newIndices;
  event->setDropAction( isDragValid( event, newIndices )? Qt::MoveAction: Qt::IgnoreAction );
  event->accept();
}

void DFGPEWidget_Elements_TableWidget::dragMoveEvent( QDragMoveEvent *event )
{
  QTableWidget::dragMoveEvent( event );
  QList<int> newIndices;
  event->setDropAction( isDragValid( event, newIndices )? Qt::MoveAction: Qt::IgnoreAction );
  event->accept();
}

void DFGPEWidget_Elements_TableWidget::dropEvent( QDropEvent *event )
{
  if ( event->source() != this
    || event->dropAction() != Qt::MoveAction )
  {
    event->ignore();
    return;
  }

  QList<int> newIndices;
  if ( isDragValid( event, newIndices ) )
    m_model->reorderElementsEventually( newIndices );

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

DFGPEWidget_Elements_PortNameDelegate::DFGPEWidget_Elements_PortNameDelegate(
  QRegExp regexFilter
  )
  : m_regexFilter( regexFilter )
{
}

QWidget* DFGPEWidget_Elements_PortNameDelegate::createEditor(
  QWidget *parent,
  QStyleOptionViewItem const &option,
  QModelIndex const &index
  ) const
{
  QLineEdit *lineEdit = new QLineEdit( parent );
  lineEdit->setValidator(
    new QRegExpValidator( m_regexFilter )
    );
  return lineEdit;
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
  QRegExp typeSpecRegExp,
  QObject *parent
  )
  : QStyledItemDelegate( parent )
  , m_client( client )
  , m_typeSpecRegExp( typeSpecRegExp )
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
  lineEdit->setValidator(
    new QRegExpValidator( m_typeSpecRegExp )
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
