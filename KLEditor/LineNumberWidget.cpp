// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "LineNumberWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <string>

#include <stdio.h>

using namespace FabricUI::KLEditor;

LineNumberWidget::LineNumberWidget(QWidget * parent, const EditorConfig & config)
: QWidget(parent)
{
  m_config = config;
  m_metrics = new QFontMetrics(m_config.lineNumberFont);
  m_lineOffset = 0;

  int maxWidth = m_metrics->width("0000") + 6;
  setMinimumWidth(maxWidth);
  setMaximumWidth(maxWidth);

  setContentsMargins(0, 0, 0, 0);
}

LineNumberWidget::~LineNumberWidget()
{
  delete(m_metrics);
}

unsigned int LineNumberWidget::lineOffset() const
{
  return m_lineOffset;
}

void LineNumberWidget::setLineOffset(unsigned int offset)
{
  if(offset != m_lineOffset)
  {
    m_lineOffset = offset;
    update();
  }
}

void LineNumberWidget::paintEvent(QPaintEvent * event)
{
  QPainter painter(this);
  
  painter.fillRect(event->rect(), m_config.lineNumberBackgroundColor);

  int width = event->rect().width();
  int height = event->rect().height();

  int offset = m_metrics->lineSpacing();
  int extraOffset = 0;
  
  if(m_lineOffset != 0)
  {
#if defined(FABRIC_OS_LINUX)
    offset -= 4;
#elif defined(FABRIC_OS_DARWIN)
    offset -= 2;
    extraOffset = 1;
#else
    offset -= 4;
#endif
  }
  else
  {
  #if defined(FABRIC_OS_DARWIN)
    offset += 1;
  #endif
  }

  painter.setFont(m_config.lineNumberFont);
  painter.setPen(m_config.lineNumberFontColor);

  // std::string tempText;
  // tempText = "0001";
  // painter.drawText(QPoint(width - 2 - lineWidth, m_metrics->lineSpacing()), tempText.c_str()); // debug

  int line = m_lineOffset + 1;
  char buffer[128];
  while(offset < height)
  {
    std::string paddingNumber;
#ifdef _WIN32
    itoa(line, buffer, 10);
#else
    snprintf(buffer, 128, "%d", line);
#endif
    paddingNumber = buffer;
    while(paddingNumber.length() < 4)
      paddingNumber = " " + paddingNumber;

    int lineWidth = m_metrics->width(paddingNumber.c_str());
    painter.drawText(QPoint(width - 2 - lineWidth, offset), paddingNumber.c_str());
    offset += m_metrics->lineSpacing() + extraOffset;
    line++;
  }

  QWidget::paintEvent(event);
}
