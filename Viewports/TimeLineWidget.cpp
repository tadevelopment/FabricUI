/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include "TimeLineWidget.h"
#include <FabricUI/Util/LoadFabricStyleSheet.h>

#include <QMenu>
#include <QInputDialog>
#include <QDesktopWidget>
#include <QLabel>

#include <algorithm>
#include <math.h>

using namespace FabricUI::TimeLine;

TimeLineWidget::TimeLineWidget()
{
  setObjectName( "DFGTimelineWidget" );
  reloadStyles();

  m_settingTime = false;

  // last frame processed
  m_lastSteppedFrame = UINT_MAX;

  // default direction is forward
  m_direction = 1;

  // default looping
  m_loopMode = LOOP_MODE_DEFAULT;

  // default playback sim
  m_simMode = 0;

  //QTimer is not precise at all; just make it call often as 
  //possible (1 ms) and we will compute the actual elapsed time
  m_timer = new QTimer(this);
  m_timer->setInterval(1);

  // layout
  setLayout(new QHBoxLayout());
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  setContentsMargins(0, 0, 0, 0);
  layout()->setContentsMargins(0, 0, 0, 0);

  // elements
  m_startSpinBox = new QDoubleSpinBox(this);
  m_startSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
  m_startSpinBox->setWrapping(false);
  m_startSpinBox->setFrame(false);
  m_startSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  m_startSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  m_startSpinBox->setDecimals(0);
  m_startSpinBox->setMinimum(-99999);
  m_startSpinBox->setMaximum(+99999);
  layout()->addWidget(m_startSpinBox);

  m_frameSlider = new FrameSlider(this);
  m_frameSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  m_frameSlider->setOrientation(Qt::Horizontal);
  m_frameSlider->setInvertedControls(false);
  m_frameSlider->setTickPosition(QSlider::TicksBelow);
  layout()->addWidget(m_frameSlider);

  m_endSpinBox = new QDoubleSpinBox(this);
  m_endSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
  m_endSpinBox->setFrame(false);
  m_endSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  m_endSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  m_endSpinBox->setDecimals(0);
  m_endSpinBox->setMinimum(-99999);
  m_endSpinBox->setMaximum(+99999);
  layout()->addWidget(m_endSpinBox);

  QLabel *separator = new QLabel;
  separator->setObjectName( "DFGTimelineSeparator" );
  separator->setContentsMargins(0, 0, 0, 0);
  layout()->addWidget(separator);

  m_currentFrameSpinBox = new QDoubleSpinBox(this);
  m_currentFrameSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
  m_currentFrameSpinBox->setFrame(false);
  m_currentFrameSpinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  m_currentFrameSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
  m_currentFrameSpinBox->setDecimals(0);
  m_currentFrameSpinBox->setMinimum(-99999);
  m_currentFrameSpinBox->setMaximum(+99999);
  layout()->addWidget(m_currentFrameSpinBox);

  m_goToStartFrameButton = new QPushButton(this);
  m_goToStartFrameButton->setObjectName( "DFGTimelineGoToStartFrame" );
  layout()->addWidget(m_goToStartFrameButton);

  m_goToPreviousFrameButton = new QPushButton(this);
  m_goToPreviousFrameButton->setObjectName( "DFGTimelineGoToPreviousFrame" );
  layout()->addWidget(m_goToPreviousFrameButton);

  m_playButton = new QPushButton(this);
  m_playButton->setObjectName( "DFGTimelinePlay" );
  m_playButton->setCheckable( true );
  layout()->addWidget(m_playButton);

  m_goToNextFrameButton = new QPushButton(this);
  m_goToNextFrameButton->setObjectName( "DFGTimelineGoToNextFrame" );
  layout()->addWidget(m_goToNextFrameButton);

  m_goToEndFrameButton = new QPushButton(this);
  m_goToEndFrameButton->setObjectName( "DFGTimelineGoToEndFrame" );
  layout()->addWidget(m_goToEndFrameButton);

  m_frameRateComboBox = new QComboBox(this);
  m_frameRateComboBox->setFrame(false);
  m_frameRateComboBox->setLayoutDirection(Qt::LeftToRight);
  m_frameRateComboBox->setEditable(false);
  m_frameRateComboBox->addItem("max fps", QVariant(0.0));
  m_frameRateComboBox->addItem("12.0 fps", QVariant(12.0));
  m_frameRateComboBox->addItem("23.98 fps", QVariant(23.98));
  m_frameRateComboBox->addItem("24.0 fps", QVariant(24.0));
  m_frameRateComboBox->addItem("25.0 fps", QVariant(25.0));  
  m_frameRateComboBox->addItem("29.97 fps", QVariant(29.97));
  m_frameRateComboBox->addItem("30.0 fps", QVariant(30.0));
  m_frameRateComboBox->addItem("48.0 fps", QVariant(48.0));
  m_frameRateComboBox->addItem("60.0 fps", QVariant(60.0));
  m_frameRateComboBox->addItem("120.0 fps", QVariant(120.0));
  m_frameRateComboBox->addItem("custom fps", QVariant(-1.0));
  layout()->addWidget(m_frameRateComboBox);

  m_loopModeComBox = new QComboBox(this);
  m_loopModeComBox->setFrame(false);
  m_loopModeComBox->setLayoutDirection(Qt::LeftToRight);
  m_loopModeComBox->setEditable(false);
  m_loopModeComBox->addItem("Play Once");
  m_loopModeComBox->addItem("Loop");
  m_loopModeComBox->addItem("Oscillate");
  m_loopModeComBox->setCurrentIndex(m_loopMode);
  layout()->addWidget(m_loopModeComBox);

  m_simModeComBox = new QComboBox(this);
  m_simModeComBox->setFrame(false);
  m_simModeComBox->setLayoutDirection(Qt::LeftToRight);
  m_simModeComBox->setEditable(false);
  m_simModeComBox->addItem("Interactive");
  m_simModeComBox->setItemData(0, "Always evaluate", Qt::ToolTipRole);
  m_simModeComBox->addItem("Simulation");
  m_simModeComBox->setItemData(1, "Evaluate all frames, also in betweens.\nReset the stepping on first frame.", Qt::ToolTipRole);
  layout()->addWidget(m_simModeComBox);

  // QLine * line = new QLine();
  // line->setOrientation(Qt::Vertical);
  // layout()->addWidget(line);

  // now we setup the connection
  connect(m_frameSlider , SIGNAL(valueChanged(int)) , this , SLOT(sliderChanged(int)));
  connect(m_currentFrameSpinBox , SIGNAL(editingFinished()) , this , SLOT(currentFrameChanged()));

  // there is an issue with doing that when we enter a value manually
  // not doing it is also an issue because then changing the value with the wheel is not doing the correct thing .
  // connect(m_currentFrameSpinBox , SIGNAL(valueChanged(double)) , this , SLOT(currentFrameChanged()));
  connect(m_startSpinBox , SIGNAL(editingFinished()) , this , SLOT(updateFrameRange()));
  connect(m_endSpinBox , SIGNAL(editingFinished()) , this , SLOT(updateFrameRange()));

  connect( m_goToStartFrameButton , SIGNAL(clicked()) , this , SLOT( goToStartFrame() ) );
  connect( m_goToPreviousFrameButton , SIGNAL(clicked()) , this , SLOT( goToPreviousFrame() )  );

  connect( m_playButton , SIGNAL(toggled(bool)) , this , SLOT( onPlayButtonToggled(bool) ) );

  connect( m_goToNextFrameButton , SIGNAL(clicked()) , this , SLOT( goToNextFrame() ) );
  connect( m_goToEndFrameButton , SIGNAL(clicked()) , this , SLOT( goToEndFrame() ) );

  connect( m_timer , SIGNAL(timeout()) , this , SLOT(timerUpdate()) );

  connect( m_frameRateComboBox , SIGNAL(activated(int)) , this , SLOT( frameRateChanged(int))  );
  connect( m_loopModeComBox , SIGNAL(activated(int)) , this , SLOT( loopModeChanged(int))  );
  connect( m_simModeComBox , SIGNAL(activated(int)) , this , SLOT( simModeChanged(int))  );

  // [FE-6693] set default framerate to 60 fps.
  setFrameRate(60.0f);
}

void TimeLineWidget::setTime(int time)
{
  if(m_settingTime)
    return;
  m_settingTime = true;

  m_frameSlider->setValue(time);
  m_currentFrameSpinBox->setValue(time);

  m_settingTime = false;

  if(m_simMode != 0)
  {
    if(m_lastSteppedFrame == int(UINT_MAX) || time == getRangeStart())
      m_lastSteppedFrame = getRangeStart()-1;

    if(m_lastSteppedFrame >= time)
      return;

    if(time > getRangeStart() && time > m_lastSteppedFrame)
    {
      // step through all frame in betweens
      for(int i=m_lastSteppedFrame+1;i<time;i++)
      {
        m_lastSteppedFrame = i;
        emit frameChanged(i);
      }
    }

    if(time > m_lastSteppedFrame || time == getRangeStart())
      m_lastSteppedFrame = time;
  }
  else
  {
    m_lastSteppedFrame = time;
  }

  emit frameChanged(time);
}

/// update the internal time and also emit the signals
void TimeLineWidget::updateTime(int frame, bool onLoadingScene)
{
  if(onLoadingScene)
    m_lastSteppedFrame = UINT_MAX;
  setTime(frame);
}

int TimeLineWidget::getTime()
{
  return static_cast<int>( m_currentFrameSpinBox->value() );
}

int TimeLineWidget::getRangeStart()
{
  return static_cast<int>( m_startSpinBox->value() );
}

int TimeLineWidget::getRangeEnd()
{
  return static_cast<int>( m_endSpinBox->value() );
}

void TimeLineWidget::sliderChanged(int frame)
{
  setTime( frame );
}

void TimeLineWidget::frameChangedBy(int frame)
{
  setTime( static_cast<int>(  m_currentFrameSpinBox->value()+frame ) );
}

void TimeLineWidget::currentFrameChanged()
{
  setTime( static_cast<int>(  m_currentFrameSpinBox->value() ) );
}

void TimeLineWidget::setTimeRange(int start, int end)
{
  m_startSpinBox->blockSignals(true);
  m_startSpinBox->setValue( static_cast<int>( start ) );
  m_startSpinBox->blockSignals(false);

  m_endSpinBox->blockSignals(true);
  m_endSpinBox->setValue( static_cast<int>( end ) );
  m_endSpinBox->blockSignals(false);

  m_lastSteppedFrame = start;

  // update the other elements
  updateFrameRange();

  // current time need to be on the bound
  int currentTime = getTime();
  if (currentTime > end)
    setTime(end);

  if (currentTime < start )
    setTime(start);
}

void TimeLineWidget::setFrameRate(float framesPerSecond) {
  // For now just clamp to existing options
  int index = 0; // max fps
  if( framesPerSecond <= 15 )
    index = 1; // 12 fps
  else if( framesPerSecond <= 30 )
    index = 4; // 25 fps
  else if( framesPerSecond <= 50 )
    index = 7; // 48 fps
  else if( framesPerSecond <= 70 )
    index = 8; // 60 fps
  frameRateChanged( index );
  m_frameRateComboBox->setCurrentIndex( index );
}

void TimeLineWidget::setLoopMode(int mode)
{
  m_loopModeComBox->setCurrentIndex(mode);
  loopModeChanged(mode);
}

void TimeLineWidget::setSimulationMode(int mode)
{
  m_simModeComBox->setCurrentIndex(mode);
  simModeChanged(mode);
}

double TimeLineWidget::getFps() const
{
  return m_fps;
}

void TimeLineWidget::setTimerFromFps(double fps)
{
  if (fps >= 200.0)
  {
    // use "max fps".
    m_fps = 1000.0;
    m_timer->setInterval(1);
    m_frameRateComboBox->setCurrentIndex(0);
  }
  else
  {
    // set the timer interval and try to find a matching framerate for it.
    for (int i=0;i<m_frameRateComboBox->count();i++)
    {
      double itemFps = m_frameRateComboBox->itemData(i).toDouble();
      if (fabs(itemFps - fps) < 0.01)
      {
        m_fps = fps;
        m_frameRateComboBox->setCurrentIndex(i);
        return;
      }
    }

    // no match found, so we set the custom fps instead.
    m_frameRateComboBox->setItemText(m_frameRateComboBox->count() - 1, "custom " + QString::number((int)fps));
    m_frameRateComboBox->setCurrentIndex(m_frameRateComboBox->count() - 1);

  }
}

double TimeLineWidget::getFrameRateFromComboBox()
{
  if (m_frameRateComboBox)  return atof(m_frameRateComboBox->currentText().toUtf8().constData());
  else                      return 0;
}

void TimeLineWidget::updateFrameRange()
{
  m_currentFrameSpinBox->setMinimum( m_startSpinBox->value() );
  m_currentFrameSpinBox->setMaximum( m_endSpinBox->value() );

  int min = static_cast<int>( m_startSpinBox->value() );
  int max = static_cast<int>( m_endSpinBox->value() );

  m_frameSlider->setMinimum( min );
  m_frameSlider->setMaximum( max );
}

void TimeLineWidget::onPlayButtonToggled( bool checked )
{
  if ( !checked && m_timer->isActive() )
  {
    m_playButton->setText( QString::fromUtf8( "\xEF\x81\x8B" ) ); /* FontAwesome > */
    m_timer->stop();
    emit playbackChanged(false);
  }
  else if ( checked && !m_timer->isActive() )
  {
    m_playButton->setText( QString::fromUtf8( "\xEF\x81\x8C" ) ); /* FontAwesome || */
    if (m_loopMode == LOOP_MODE_PLAY_ONCE && getTime() >=  m_endSpinBox->value())
      goToStartFrame();
    m_timer->start();
    m_lastFrameTime.start();
    emit playbackChanged(true);
  }
}

void TimeLineWidget::pause()
{
  if ( m_timer->isActive() )
  {
    m_timer->stop();
    m_playButton->setChecked( false );
    emit playbackChanged(false);
  }
}

void TimeLineWidget::goToStartFrame()
{
  int newFrame = static_cast<int>( m_startSpinBox->value() );
  setTime(newFrame);
}

void TimeLineWidget::goToPreviousFrame()
{
  int newFrame = static_cast<int>( m_startSpinBox->value() );
  if(newFrame < getTime() - 1)
    newFrame = getTime() - 1;
  setTime(newFrame);
}

void TimeLineWidget::goToNextFrame()
{
  int newFrame = static_cast<int>( m_endSpinBox->value() );
  if(newFrame > getTime() + 1)
    newFrame = getTime() + 1;
  setTime(newFrame);
}

void TimeLineWidget::goToEndFrame()
{
  int newFrame = static_cast<int>( m_endSpinBox->value() );
  setTime(newFrame);
}

void TimeLineWidget::timerUpdate()
{
  // We will be getting about 1 call per milli-second,
  // however QTimer is really not precise so we cannot rely
  // on its delay.
  double ms = m_lastFrameTime.elapsed();
  if( m_fps > 0 && ms + 0.5 < 1000.0 / m_fps ) // Add 0.5 so we have a better average framerate (else we are always above)
    return; // Wait longer

  m_lastFrameTime.start();

  int newTime = getTime()+m_direction;

  // case 1: new time is inside the time range.
  if (newTime >= m_startSpinBox->value() && newTime <= m_endSpinBox->value())
  {
    setTime(newTime);
  }

  // case 2: new time is outside the time range.
  else
  {
    // case 2a: new time is greater than the time range end.
    if (newTime > m_endSpinBox->value())
    {
      switch (m_loopMode)
      {
        case LOOP_MODE_PLAY_ONCE:
        {
          pause();
        } break;
        case LOOP_MODE_LOOP:
        {
          goToStartFrame();
        } break;
        case LOOP_MODE_OSCILLATE:
        {
          m_direction = -1;
          newTime = int(m_endSpinBox->value() + double(m_direction));
          setTime(newTime);
        } break;
        default:
        {
          pause();
        } break;
      }
    }

    // case 2b: new time is less than the time range start.
    else
    {
      switch (m_loopMode)
      {
        case LOOP_MODE_OSCILLATE:
        {
          m_direction = 1;
          newTime = int(m_startSpinBox->value() + double(m_direction));
          setTime(newTime);
        } break;
        default:
        {
          m_direction = 1;
          goToStartFrame();
        } break;
      }
    }
  }
}

void TimeLineWidget::frameRateChanged(int index)
{
  m_fps = m_frameRateComboBox->itemData(index).toDouble();
  if(m_fps == 0.0) // max fps
    m_fps = 1000; // max fps
  else if(m_fps == -1.0) // custom fps
  {
    bool ok;
    double userFps = QInputDialog::getDouble(
      this, tr("Custom FPS"), tr(""),
      24.0, 1.0, 1000.0, 2, &ok );
    if(ok)
    {
      m_fps = userFps;
      m_frameRateComboBox->setItemText( index, 
        "custom " + QString::number(m_fps) );
    }
  }
}

void TimeLineWidget::loopModeChanged(int index)
{
  m_loopMode = index;
  if (index != 2)
    m_direction = 1;
}

void TimeLineWidget::simModeChanged(int index)
{
  m_simMode = index;
}

void TimeLineWidget::reloadStyles()
{
  QString styleSheet = LoadFabricStyleSheet( "FabricUI.qss" );
  if ( !styleSheet.isEmpty() )
    setStyleSheet( styleSheet );
}
