//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValFCurveViewItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/AnimXKL/RTValAnimXFCurveModel.h>

#include <FabricUI/ModelItems/DFGModelItemMetadata.h>
#include <FabricUI/Util/QtSignalsSlots.h>
#include <FabricUI/Util/LoadPixmap.h>

#include <FabricUI/Commands/CommandManager.h>
#include <FabricUI/Commands/KLCommandRegistry.h> // HACK: remove

#include <QPushButton>
#include <QDialog>
#include <QLayout>

#include <assert.h>
#include <QDebug>

using namespace FabricUI::ValueEditor;
using namespace FabricUI::FCurveEditor;

class RTValFCurveViewItem::RTValAnimXFCurveDFGController : public RTValAnimXFCurveVersionedConstModel
{
  std::string m_bindingId, m_dfgPortPath;
  size_t m_interactionId;

  QString m_lastCommand;
  QMap<QString, QString> m_lastArgs;

  inline void synchronizeKLReg()
  {
    static_cast<FabricUI::Commands::KLCommandRegistry*>( FabricUI::Commands::KLCommandRegistry::getCommandRegistry() )
      ->synchronizeKL(); // HACK : remove
  }

public:
  void setPath( const char* bindingId, const char* dfgPortPath )
  {
    m_bindingId = bindingId;
    m_dfgPortPath = dfgPortPath;
  }

  void setKey( size_t i, Key h ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    FabricCore::RTVal bRV = FabricCore::RTVal::ConstructBoolean( m_val.getContext(), true );
    const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "", "useIds", 1, &bRV );
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["id"] = QString::number( i );
    args["time"] = QString::number( h.pos.x() );
    args["value"] = QString::number( h.pos.y() );
    args["tanInX"] = QString::number( h.tanIn.x() );
    args["tanInY"] = QString::number( h.tanIn.y() );
    args["tanOutX"] = QString::number( h.tanOut.x() );
    args["tanOutY"] = QString::number( h.tanOut.y() );
    QString cmdName = "AnimX_SetKeyframe";
    manager->createCommand( cmdName, args, true, m_interactionId );
    m_lastCommand = cmdName;
    m_lastArgs = args;
    emit this->dirty();
  }

  inline QString serializeQS( const size_t* indices, const size_t nbIndices )
  {
    QString indicesStr = "[";
    for( size_t i = 0; i < nbIndices; i++ )
    {
      if( i > 0 )
        indicesStr += ",";
      indicesStr += QString::number( indices[i] );
    }
    indicesStr += "]";
    return indicesStr;
  }

  void moveKeys( const size_t* indices, const size_t nbIndices, QPointF delta ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    FabricCore::RTVal bRV = FabricCore::RTVal::ConstructBoolean( m_val.getContext(), true );
    const_cast<FabricCore::RTVal*>( &m_val )->callMethod( "", "useIds", 1, &bRV );
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["ids"] = serializeQS( indices, nbIndices );
    args["dx"] = QString::number( delta.x() );
    args["dy"] = QString::number( delta.y() );
    QString cmdName = "AnimX_MoveKeyframes";
    manager->createCommand( cmdName, args, true, m_interactionId );
    m_lastCommand = cmdName;
    m_lastArgs = args;
    emit this->dirty();
  }

  void addKey() FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    manager->createCommand( "AnimX_PushKeyframe", args );
    emit this->dirty();
  }

  void deleteKey( size_t i ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["id"] = QString::number( i );
    manager->createCommand( "AnimX_RemoveKeyframe", args );
    emit this->dirty();
  }

  void deleteKeys( const size_t* indices, const size_t nbIndices ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["ids"] = serializeQS( indices, nbIndices );
    manager->createCommand( "AnimX_RemoveKeyframes", args );
    emit this->dirty();
  }

  inline void incrementInteractionId()
  {
    m_interactionId = FabricUI::Commands::CommandManager::getCommandManager()->getNewCanMergeID();
  }

  void onInteractionEnd()
  {
    if( !m_lastCommand.isEmpty() )
    {
      FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
      this->synchronizeKLReg();
      QMap<QString, QString> args = m_lastArgs;
      args["interactionEnd"] = "true";
      manager->createCommand( m_lastCommand, args, true, m_interactionId );
      m_lastCommand = "";
      emit this->dirty();
    }
  }

  inline void emitDirty() { emit this->dirty(); }
};

class RTValFCurveViewItem::Editor : public FabricUI::FCurveEditor::FCurveEditor
{
  typedef FabricUI::FCurveEditor::FCurveEditor Parent;
  RTValFCurveViewItem* m_parent;
  QPushButton* m_expandButton;
  void updateExpandButtonSize()
  {
    QRect cr = this->contentsRect();
    m_expandButton->setGeometry( QRect(
      cr.right() - 8 - m_expandButton->width(),
      cr.top() + 8,
      m_expandButton->width(),
      m_expandButton->height()
    ) );
    this->setVEPos( QPoint( -8 - m_expandButton->width() - 8, 8 ) );
  }
public:
  Editor( RTValFCurveViewItem* parent )
    : m_parent( parent )
    , m_expandButton( new QPushButton( this ) )
  {
    m_expandButton->setObjectName( "expand" );
    m_expandButton->resize( 32, 32 );
    QOBJECT_CONNECT( m_expandButton, SIGNAL, QPushButton, released, ( ), m_parent, SLOT, RTValFCurveViewItem, expand, ( ) );
    this->updateExpandButtonSize();
  }
  void resizeEvent( QResizeEvent * e ) FTL_OVERRIDE
  {
    Parent::resizeEvent( e );
    this->updateExpandButtonSize();
    this->frameAllKeys();
  }
};

class RTValFCurveViewItem::ExpandedDialog : public QWidget
{
  FabricUI::FCurveEditor::FCurveEditor* m_editor;
public:
  ExpandedDialog( Editor* parent )
    : QWidget( parent, Qt::Window )
    , m_editor( new FabricUI::FCurveEditor::FCurveEditor() )
  {
    QHBoxLayout* m_lay = new QHBoxLayout();
    m_lay->setMargin( 0 ); m_lay->setSpacing( 0 );
    m_lay->addWidget( m_editor );
    this->setLayout( m_lay );
  }
  inline FabricUI::FCurveEditor::FCurveEditor* editor() { return m_editor; }
};

RTValFCurveViewItem::RTValFCurveViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
) : BaseViewItem( name, metadata )
  , m_model( new RTValAnimXFCurveDFGController() )
  , m_editor( new Editor( this ) )
  , m_expandedDialog( new ExpandedDialog( m_editor ) )
{
  m_editor->setModel( m_model );
  m_expandedDialog->editor()->setModel( m_model );
  this->onModelValueChanged( value );
  m_editor->setToolBarEnabled( false );
  m_expandedDialog->editor()->setToolBarEnabled( true );

  m_editor->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed ) );
  m_editor->setFixedHeight( 175 );
  m_editor->frameAllKeys( );

  //connect( m_model, SIGNAL( keyAdded() ), this, SLOT( onViewValueChanged() ) );
  //connect( m_model, SIGNAL( keyMoved( size_t ) ), this, SLOT( onViewValueChanged() ) );
  //connect( m_editor, SIGNAL( interactionBegin() ), this, SIGNAL( interactionBegin() ) );
  //connect( m_editor, SIGNAL( interactionEnd() ), this, SLOT( emitInteractionEnd() ) );

  for( size_t i = 0; i < 2; i++ )
  {
    FabricUI::FCurveEditor::FCurveEditor* editor =
      ( i == 0 ? m_editor : m_expandedDialog->editor() );
    QOBJECT_CONNECT(
      editor, SIGNAL, FabricUI::FCurveEditor::FCurveEditor, interactionBegin, ( ),
      this, SLOT, RTValFCurveViewItem, onEditorInteractionBegin, ( )
    );
    QOBJECT_CONNECT(
      editor, SIGNAL, FabricUI::FCurveEditor::FCurveEditor, interactionEnd, ( ),
      this, SLOT, RTValFCurveViewItem, onEditorInteractionEnd, ( )
    );
  }

  const char* bindingId = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGBindingIdKey.data() );
  const char* portPath = metadata->getString( FabricUI::ModelItems::DFGModelItemMetadata::VEDFGPortPathKey.data() );
  m_model->setPath( bindingId, portPath );

  m_expandedDialog->setWindowTitle( "AnimX::AnimCurve <" + QString::fromUtf8( portPath ) + ">" );
}

void RTValFCurveViewItem::onEditorInteractionBegin()
{
  m_model->incrementInteractionId();
}

void RTValFCurveViewItem::onEditorInteractionEnd()
{
  m_model->onInteractionEnd();
}

void RTValFCurveViewItem::expand()
{
  m_expandedDialog->show();
  m_expandedDialog->editor()->frameAllKeys( );
}

RTValFCurveViewItem::~RTValFCurveViewItem()
{
  delete m_editor;
  delete m_model;
}

void RTValFCurveViewItem::onViewValueChanged()
{
  //emit this->viewValueChanged( QVariant::fromValue<FabricCore::RTVal>( m_model->value() ) );
}

QWidget* RTValFCurveViewItem::getWidget()
{
  return m_editor;
}

void RTValFCurveViewItem::onModelValueChanged( QVariant const & v )
{
  FabricCore::RTVal rtval = v.value<FabricCore::RTVal>();
  m_model->setValue( rtval );
  m_model->update();
  m_model->emitDirty();
}

BaseViewItem * RTValFCurveViewItem::CreateItem(
  QString const& name,
  QVariant const& value,
  ItemMetadata* metaData
)
{
  if( isRTVal( value ) )
  {
    const FabricCore::RTVal& val = value.value<FabricCore::RTVal>();
    if( val.isValid() )
    {
      const char* rtype = val.getTypeNameCStr();
      if( strcmp( rtype, "AnimX::AnimCurve" ) == 0 )
        return new RTValFCurveViewItem( name, value, metaData );
    }
  }
  return NULL;
}

const int RTValFCurveViewItem::Priority = 3;
