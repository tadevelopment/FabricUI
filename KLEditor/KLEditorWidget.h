// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_KLEditorWidget__
#define __UI_KLEditor_KLEditorWidget__

#include <QFrame>
#include "LineNumberWidget.h"
#include "KLSourceCodeWidget.h"

namespace FabricUI
{

  namespace KLEditor
  {

    class KLEditorWidget : public QFrame
    {
      Q_OBJECT
    
    public:
    
      KLEditorWidget(
        QWidget * parent,
        FabricServices::ASTWrapper::KLASTManager * manager,
        const EditorConfig & config = EditorConfig()
        );
      virtual ~KLEditorWidget();

      KLSourceCodeWidget * sourceCodeWidget();

    private:
      EditorConfig m_config;
      LineNumberWidget * m_lineNumbers;
      KLSourceCodeWidget * m_sourceCodeWidget;
    };

  };

};

#endif // __UI_KLEditor_KLEditorWidget__
