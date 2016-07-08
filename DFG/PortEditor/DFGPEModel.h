//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEModel_h
#define _DFGPEModel_h

#include <FabricCore.h>
#include <FTL/ArrayRef.h>
#include <QtCore/QObject>
#include <QtCore/QTimer>

namespace FabricUI {
namespace DFG {

class DFGWidget;

class DFGPEModel : public QObject
{
  Q_OBJECT

public:

  virtual bool isPortTypeFixed() = 0;
  bool isReadOnly() { return m_isReadOnly; }

  virtual int getPortCount() = 0;
  virtual QString getPortName( int index ) = 0;
  virtual FabricCore::DFGPortType getPortType( int index ) = 0;
  virtual QString getPortTypeSpec( int index ) = 0;
  bool isPortReadOnly( int index )
    { return isReadOnly() || isPortReadOnlyImpl( index ); }

  virtual void insertPort(
    int index,
    QString desiredName,
    FabricCore::DFGPortType type,
    QString typeSpec
    ) = 0;
  virtual void inspectPort(
    int index,
    DFGWidget *dfgWidget
    ) = 0;
  virtual void renamePort(
    int index,
    QString newName
    ) = 0;
  virtual void setPortType(
    int index,
    FabricCore::DFGPortType type
    ) = 0;
  virtual void setPortTypeSpec(
    int index,
    QString newTypeSpec
    ) = 0;
  virtual void removePort(
    int index
    ) = 0;

  virtual void reorderPorts(
    QList<int> newIndices
    ) = 0;

  void reorderPortsEventually( QList<int> newIndices );

signals:
  
  void isReadOnlyChanged(
    bool newIsReadOnly
    );
  
  void portInserted(
    int index,
    QString name,
    FabricCore::DFGPortType type,
    QString typeSpec
    );
  void portRenamed(
    int index,
    QString newName
    );
  void portTypeChanged(
    int index,
    FabricCore::DFGPortType type
    );
  void portTypeSpecChanged(
    int index,
    QString newTypeSpec
    );
  void portRemoved(
    int index
    );

  void portsReordered(
    QList<int> newIndices
    );

protected:

  void init()
  {
    m_isReadOnly = computeIsReadOnly();
  }

  virtual bool isPortReadOnlyImpl( int index ) = 0;
  virtual bool computeIsReadOnly() = 0;

  void updateIsReadOnly()
  {
    bool newIsReadOnly = computeIsReadOnly();
    if ( newIsReadOnly != m_isReadOnly )
    {
      m_isReadOnly = newIsReadOnly;
      emit isReadOnlyChanged( m_isReadOnly );
    }
  }

private:

  bool m_isReadOnly;
};

class DFGPEModel_ReorderPortsEventallyTimer : QTimer
{
  Q_OBJECT

public:

  DFGPEModel_ReorderPortsEventallyTimer(
    DFGPEModel *model,
    QList<int> newIndices
    );

protected slots:

  void onTimeout();

private:

  DFGPEModel *m_model;
  QList<int> m_newIndices;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEModel_h
