//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEModel_h
#define _DFGPEModel_h

#include <FabricCore.h>
#include <FTL/ArrayRef.h>
#include <QObject>
#include <QTimer>

namespace FabricUI {
namespace DFG {

class DFGWidget;

class DFGPEModel : public QObject
{
  Q_OBJECT

public:

  virtual bool canInspectElements() = 0;
  virtual bool hasPortType() = 0;
  virtual bool hasTypeSpec() = 0;
  virtual bool allowReordering() = 0;
  bool isReadOnly() { return m_isReadOnly; }

  QString getElementDesc() const
    { return m_elementDesc; }
  QString getElementDescCapitalized() const
    { return m_elementDescCapitalized; }

  virtual int getElementCount() = 0;
  virtual QString getElementName( int index ) = 0;
  virtual FabricCore::DFGPortType getElementPortType( int index ) = 0;
  virtual QString getElementTypeSpec( int index ) = 0;
  bool isElementReadOnly( int index )
    { return isReadOnly() || isElementReadOnlyImpl( index ); }

  virtual void insertElement(
    int index,
    QString desiredName,
    FabricCore::DFGPortType type,
    QString typeSpec
    ) = 0;
  virtual void inspectElement(
    int index,
    DFGWidget *dfgWidget
    ) = 0;
  virtual void renameElement(
    int index,
    QString newName
    ) = 0;
  virtual void setElementPortType(
    int index,
    FabricCore::DFGPortType type
    ) = 0;
  virtual void setElementTypeSpec(
    int index,
    QString newTypeSpec
    ) = 0;
  virtual void removeElements(
    QList<int> indices
    ) = 0;

  virtual void reorderElements(
    QList<int> newIndices
    ) = 0;

  void reorderElementsEventually( QList<int> newIndices );

signals:
  
  void isReadOnlyChanged(
    bool newIsReadOnly
    );
  
  void elementInserted(
    int index,
    QString name,
    FabricCore::DFGPortType type,
    QString typeSpec
    );
  void elementRenamed(
    int index,
    QString newName
    );
  void elementPortTypeChanged(
    int index,
    FabricCore::DFGPortType type
    );
  void elementTypeSpecChanged(
    int index,
    QString newTypeSpec
    );
  void elementRemoved(
    int index
    );

  void elementsReordered(
    QList<int> newIndices
    );

protected:

  DFGPEModel(
    QString elementDesc,
    QString elementDescCapitalized
    );

  void init()
  {
    m_isReadOnly = computeIsReadOnly();
  }

  virtual bool isElementReadOnlyImpl( int index ) = 0;
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

  QString m_elementDesc;
  QString m_elementDescCapitalized;
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
