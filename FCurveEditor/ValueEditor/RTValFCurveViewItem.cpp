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

  void setHandle( size_t i, Handle h ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
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

  void moveHandles( const size_t* indices, const size_t nbIndices, QPointF delta ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["ids"] = serializeQS( indices, nbIndices );
    args["dx"] = QString::number( delta.x() );
    args["dy"] = QString::number( delta.y() );
    QString cmdName = "AnimX_MoveKeyframes";
    manager->createCommand( cmdName, args, true, m_interactionId );
    m_lastCommand = cmdName;
    m_lastArgs = args;
  }

  void addHandle() FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    manager->createCommand( "AnimX_PushKeyframe", args );
  }

  void deleteHandle( size_t i ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["id"] = QString::number( i );
    manager->createCommand( "AnimX_RemoveKeyframe", args );
  }

  void deleteHandles( const size_t* indices, const size_t nbIndices ) FTL_OVERRIDE
  {
    FabricUI::Commands::CommandManager* manager = FabricUI::Commands::CommandManager::getCommandManager();
    this->synchronizeKLReg();
    QMap<QString, QString> args;
    args["target"] = "<" + QString::fromUtf8( m_bindingId.data() ) + "." + QString::fromUtf8( m_dfgPortPath.data() ) + ">";
    args["ids"] = serializeQS( indices, nbIndices );
    manager->createCommand( "AnimX_RemoveKeyframes", args );
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
    }
  }
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
    m_expandButton->setIcon( QIcon( LoadPixmap( "expand.png" ) ) );
    m_expandButton->setStyleSheet( "background-color: rgba(0,0,0,0);" );
    m_expandButton->resize( 32, 32 );
    QOBJECT_CONNECT( m_expandButton, SIGNAL, QPushButton, pressed, ( ), m_parent, SLOT, RTValFCurveViewItem, expand, ( ) );
    this->updateExpandButtonSize();
  }
  void resizeEvent( QResizeEvent * e ) FTL_OVERRIDE
  {
    Parent::resizeEvent( e );
    this->updateExpandButtonSize();
  }
};

class RTValFCurveViewItem::ExpandedEditor : public FabricUI::FCurveEditor::FCurveEditor
{

};

RTValFCurveViewItem::RTValFCurveViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
) : BaseViewItem( name, metadata )
  , m_model( new RTValAnimXFCurveDFGController() )
  , m_editor( new Editor( this ) )
  , m_expandedEditor( new ExpandedEditor() )
{
  m_editor->setModel( m_model );
  m_expandedEditor->setModel( m_model );
  this->onModelValueChanged( value );

  m_editor->setFixedSize( 500, 150 ); // HACK
  m_editor->fitInView();

  //connect( m_model, SIGNAL( handleAdded() ), this, SLOT( onViewValueChanged() ) );
  //connect( m_model, SIGNAL( handleMoved( size_t ) ), this, SLOT( onViewValueChanged() ) );
  //connect( m_editor, SIGNAL( interactionBegin() ), this, SIGNAL( interactionBegin() ) );
  //connect( m_editor, SIGNAL( interactionEnd() ), this, SLOT( emitInteractionEnd() ) );

  for( size_t i = 0; i < 2; i++ )
  {
    FabricUI::FCurveEditor::FCurveEditor* editor =
      ( i == 0 ? m_editor : ( FabricUI::FCurveEditor::FCurveEditor* )m_expandedEditor );
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

  m_expandedEditor->setWindowTitle( "AnimX::AnimCurve <" + QString::fromUtf8( portPath ) + ">" );
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
  m_expandedEditor->show();
  m_expandedEditor->fitInView();
}

RTValFCurveViewItem::~RTValFCurveViewItem()
{
  //delete m_editor; // HACK;TODO : delete and fix crash when closing Canvas
  delete m_expandedEditor;
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
