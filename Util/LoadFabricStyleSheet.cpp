//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/Util/LoadFabricStyleSheet.h>
#include <FabricUI/Util/FabricResourcePath.h>

#include <FTL/Path.h>
#include <QFile>
#include <QRegExp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

static QString ExpandEnvironmentVariables( QString string )
{
  static bool initialized = false;
  static QRegExp re;
  if ( !initialized )
  {
    initialized = true;
    re = QRegExp( "\\$\\{([^}]+)\\}" );
    re.setMinimal( true );
  }

  QString result;

  int offset = 0;
  for (;;)
  {
    int pos = re.indexIn( string, offset );
    if ( pos == -1 )
      break;

    if ( pos > offset )
      result += string.midRef( offset, pos - offset );

    QString envvarName = re.cap( 1 );
    char const *envvarValue = getenv( envvarName.toLatin1().constData() );
    if ( envvarValue )
    {
#if defined(FTL_PLATFORM_WINDOWS)
      result += QString::fromLatin1( envvarValue ).replace( '\\', '/' );
#else
      result += QString::fromUtf8( envvarValue );
#endif
    }

    offset = pos + re.matchedLength();
  }

  result += string.midRef( offset );

  return result;
}

QString LoadFabricStyleSheet( FTL::StrRef basename )
{
  QString qssPath = FabricResourcePathQS( FTL_STR("QSS"), basename );
  
  QFile qssFile( qssPath );
  if ( !qssFile.open( QFile::ReadOnly ) )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: unable to open '%s'\n",
      qssPath.toUtf8().constData()
      );
    return QString();
  }
  QByteArray qssFileBytes = qssFile.readAll();

  QString styleSheet = QString::fromUtf8( qssFileBytes.constData(), qssFileBytes.size() );
  if ( styleSheet.isEmpty() )
  {
    fprintf(
      stderr,
      "VETreeWidget::reloadStyles: style sheet '%s' is empty\n",
      qssPath.toUtf8().constData()
      );
  }
  return ExpandEnvironmentVariables( styleSheet );
}
