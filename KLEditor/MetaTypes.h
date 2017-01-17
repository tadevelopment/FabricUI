// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_MetaTypes__
#define __UI_KLEditor_MetaTypes__

#include <ASTWrapper/KLDecl.h>
#include <QFont>
#include <QTextCharFormat>

namespace FabricUI
{

  namespace KLEditor
  {

    struct KLDeclPtr
    {
      const FabricServices::ASTWrapper::KLDecl * decl;
    };

  };

};

Q_DECLARE_METATYPE(FabricUI::KLEditor::KLDeclPtr);

#endif // __UI_KLEditor_MetaTypes__
