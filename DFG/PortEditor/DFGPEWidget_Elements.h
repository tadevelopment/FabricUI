//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEWidget_Elements_h
#define _DFGPEWidget_Elements_h

#include <FabricCore.h>
#include <QtCore/QTimer>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QIcon>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QProxyStyle>
#include <QtGui/QTableWidget>

class QPushButton;

namespace FabricUI {
namespace DFG {

class DFGPEModel;
class DFGRegisteredTypeLineEdit;
class DFGWidget;

class DFGPEWidget_Elements : public QWidget
{
  Q_OBJECT

public:

  DFGPEWidget_Elements(
    DFGWidget *dfgWidget,
    DFGPEModel *model,
    QWidget *parent = NULL
    );

  void setModel( DFGPEModel *newModel );

  QStringList portTypeLabels() const
    { return m_portTypeLabels; }

protected:

  class IgnoreCellChangedBracket
  {
  public:

    IgnoreCellChangedBracket( DFGPEWidget_Elements *widget )
      : m_widget( widget )
      { m_widget->m_ignoreCellChanged = true; };

    ~IgnoreCellChangedBracket()
      { m_widget->m_ignoreCellChanged = false; };

  private:

    DFGPEWidget_Elements *m_widget;
  };

  void populatePorts();
  void unpopulatePorts();

protected slots:

  void onIsReadOnlyChanged(
    bool newIsReadOnly
    );

  void onPortInserted(
    int index,
    QString name,
    FabricCore::DFGPortType type,
    QString typeSpec
    );

  void onPortRenamed(
    int index,
    QString newPortName
    );

  void onPortTypeChanged(
    int index,
    FabricCore::DFGPortType type
    );

  void onPortTypeSpecChanged(
    int index,
    QString typeSpec
    );

  void onPortRemoved(
    int index
    );

  void onPortsReordered(
    QList<int> newIndices
    );

  void onCellChanged( int row, int col );

  void onInspectRowClicked(
    int row
    );

  void onDeleteRowClicked(
    int row
    );

  void onAddPortClicked();

private:

  DFGWidget *m_dfgWidget;
  DFGPEModel *m_model;
  bool m_ignoreCellChanged;


  QStringList m_portTypeLabels;
  QPixmap m_plusPixmap;
  QPixmap m_dotsPixmap;
  QPixmap m_minusPixmap;
  QIcon m_plusIcon;
  QIcon m_dotsIcon;
  QIcon m_minusIcon;

  bool m_isPortTypeFixed;
  int m_controlCol;
  int m_portNameCol;
  int m_portTypeCol;
  int m_portTypeSpecCol;

  QLayout *m_layout;
  QTableWidget *m_tableWidget;
  QLineEdit *m_addPortName;
  QComboBox *m_addPortType;
  DFGRegisteredTypeLineEdit *m_addPortTypeSpec;
  QPushButton *m_addPortButton;
  QFrame *m_addPortContainer;
};

class DFGPEWidget_Elements_TableWidget_ProxyStyle
  : public QProxyStyle
{
public:

  DFGPEWidget_Elements_TableWidget_ProxyStyle(
    QStyle* style = NULL
    );

  void drawPrimitive(
    PrimitiveElement element,
    QStyleOption const *option,
    QPainter *painter,
    QWidget const *widget = NULL
    ) const;
};

class DFGPEWidget_Elements_TableWidget : public QTableWidget
{
  Q_OBJECT

public:

  DFGPEWidget_Elements_TableWidget(
    DFGPEModel *model,
    int rows,
    int cols,
    QWidget *parent = NULL
    );

protected:

  bool isDragValid( QDropEvent *event );

  virtual void dragEnterEvent( QDragEnterEvent *event ) /*override*/;
  virtual void dragMoveEvent( QDragMoveEvent *event ) /*override*/;
  virtual void dropEvent( QDropEvent *event ) /*override*/;

private:

  DFGPEModel *m_model;
};

class DFGPEWidget_Elements_ControlCell : public QFrame
{
  Q_OBJECT

public:

  DFGPEWidget_Elements_ControlCell(
    int row,
    QIcon iconOne,
    QIcon iconTwo,
    QWidget *parent = NULL
    );

  void setRow( int row )
    { m_row = row; }

signals:
  
  void oneClicked( int row );
  void twoClicked( int row );

protected slots:

  void onOneClicked();
  void onTwoClicked();

private:

  int m_row;
};

class DFGPEWidget_Elements_PortTypeDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:

  DFGPEWidget_Elements_PortTypeDelegate(
    QStringList portTypeLabels,
    QObject *parent
    );

  QWidget *createEditor(
    QWidget *parent,
    QStyleOptionViewItem const &option,
    QModelIndex const &index
    ) const;

  void setEditorData(
    QWidget *editor,
    QModelIndex const &index
    ) const;

  void setModelData(
    QWidget *editor,
    QAbstractItemModel *model,
    QModelIndex const &index
    ) const;

  void updateEditorGeometry(
    QWidget *editor,
    QStyleOptionViewItem const &option,
    QModelIndex const &index
    ) const;

private:

  QStringList m_portTypeLabels;
};

class DFGPEWidget_Elements_PortTypeDelegate_ComboxBox : public QComboBox
{
  Q_OBJECT

public:

  DFGPEWidget_Elements_PortTypeDelegate_ComboxBox( QWidget *parent = NULL );

protected slots:

  void onActivated( int index );
};

class DFGPEWidget_Elements_PortTypeSpecDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:

  DFGPEWidget_Elements_PortTypeSpecDelegate(
    FabricCore::Client client,
    QObject *parent
    );

  QWidget *createEditor(
    QWidget *parent,
    QStyleOptionViewItem const &option,
    QModelIndex const &index
    ) const;

  void setEditorData(
    QWidget *editor,
    QModelIndex const &index
    ) const;

  void setModelData(
    QWidget *editor,
    QAbstractItemModel *model,
    QModelIndex const &index
    ) const;

  void updateEditorGeometry(
    QWidget *editor,
    QStyleOptionViewItem const &option,
    QModelIndex const &index
    ) const;

private:

  FabricCore::Client m_client;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEWidget_Elements_h
