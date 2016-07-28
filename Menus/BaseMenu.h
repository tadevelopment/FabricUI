/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_BASEMENU_H__
#define __FABRICUI_BASEMENU_H__

#include <QMenu>
#include <FabricCore.h>

namespace FabricUI {
namespace Menus {

class BaseMenu : public QMenu {
  
  /**
    BaseMenu is a base class for any Menu defined in C++ or Python.
    It's a basic specialization of the QMenu class.

    The class defines the constructMenu method where the Menu items 
    should be created. Construct the menu in this mehod instead of 
    within the constructor allows inheritance.

    Thus, it's a two steps construction :
      BaseMenu *menu = BaseMenu(client, "", 0);
      mennu->constructMenu();
  */

  Q_OBJECT
 
  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param title The menu title, can be None.
    /// \param parent The menu parent, can be null.
    BaseMenu(
      FabricCore::Client client, 
      QString title, 
      QWidget *parent);
    
    /// Destructor.
    virtual ~BaseMenu();

    /// Gets a reference to the Fabric Client.
    FabricCore::Client getClient();

    /// Construct the Menu, to override.        
    virtual void constructMenu();
    

  protected:
    /// A reference to the FabricCore::Client.
    FabricCore::Client m_client;
};

} // Menus
} // FabricUI 

#endif // __FABRICUI_BASEMENU_H__
