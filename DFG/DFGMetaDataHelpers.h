//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_DFG_DFGMetaDataHelpers_H
#define FABRICUI_DFG_DFGMetaDataHelpers_H

#include <FTL/StrRef.h>
#include <FTL/JSONEnc.h>
#include <QColor>

namespace FabricUI {
namespace DFG {

inline void DFGAddMetaDataPair(
  FTL::JSONObjectEnc<>& metaDataObjectEnc,
  FTL::StrRef key,
  FTL::StrRef value
  )
{
  FTL::JSONEnc<> enc( metaDataObjectEnc, key );
  FTL::JSONStringEnc<> valueEnc( enc, value );
}

inline void DFGAddMetaDataPair_Color(
  FTL::JSONObjectEnc<>& metaDataObjectEnc,
  FTL::StrRef key,
  QColor const &color
  )
{
  std::string value;
  {
    FTL::JSONEnc<> enc( value );
    FTL::JSONObjectEnc<> objEnc( enc );
    {
      FTL::JSONEnc<> rEnc( objEnc, FTL_STR("r") );
      FTL::JSONSInt32Enc<> rS32Enc( rEnc, color.red() );
    }
    {
      FTL::JSONEnc<> gEnc( objEnc, FTL_STR("g") );
      FTL::JSONSInt32Enc<> gS32Enc( gEnc, color.green() );
    }
    {
      FTL::JSONEnc<> bEnc( objEnc, FTL_STR("b") );
      FTL::JSONSInt32Enc<> bS32Enc( bEnc, color.blue() );
    }
  }

  DFGAddMetaDataPair( metaDataObjectEnc, key, value );
}

} // namespace DFG
} // namespace FabricUI

#endif // FABRICUI_DFG_DFGMetaDataHelpers_H
