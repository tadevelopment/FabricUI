cmake_minimum_required (VERSION 2.6)
project (DFGPresetSearchWidget)

set(
  FabricStageDir "${FABRIC_DIR}"
  CACHE PATH "desc" )

set(
  QtDir "C:/Qt5/5.7/msvc2013_64"
  CACHE PATH "desc" )

add_definitions(-DFEC_SHARED)

add_custom_target(
  qtMoc
  COMMAND "${CMAKE_CURRENT_LIST_DIR}/moc.bat" ${QtDir} ${CMAKE_CURRENT_LIST_DIR}
)

set( SGDir "../../../../.." )

include_directories(
  ../
  ${SGDir}
  ${SGDir}/Native
  ${SGDir}/Core/Clients/CAPI
  ${SGDir}/Native/FabricServices
  ${QtDir}/include ${QtDir}/include/QtCore
  ${QtDir}/include/QtGUI
  ${QtDir}/include/ActiveQt
  ${QtDir}/include/QtWidgets
  )

link_directories(
  ${FabricStageDir}/lib
  ${QtDir}/lib
  )

add_executable(
  DFGPresetSearchWidget
  ../DFGPresetSearchWidget.cpp
  ../DFGPresetSearchWidget.h
  ../QueryEdit.cpp
  ../QueryEdit.h
  ../ResultsView.cpp
  ../ResultsView.h
  ../DetailsWidget.cpp
  ../DetailsWidget.h
  ../ItemView.cpp
  ../ItemView.h
  ../HeatBar.cpp
  ../HeatBar.h
  ../Data.cpp
  ../Data.h
  ${SGDir}/Native/FabricUI/Style/FabricStyle.cpp
  ${SGDir}/Native/FabricUI/Util/FabricResourcesPath.cpp
  ${SGDir}/Native/FabricUI/Util/Config.cpp
  Moc.cpp
  Main.cpp
  )

configure_file( ${CMAKE_CURRENT_LIST_DIR}/../../../FabricUI.qss ${CMAKE_BINARY_DIR}/ )

add_dependencies( DFGPresetSearchWidget qtMoc )

target_link_libraries(
  DFGPresetSearchWidget
  FabricCore-2.6.lib
  Qt5Gui.lib
  Qt5Widgets.lib
  Qt5Core.lib
  )
