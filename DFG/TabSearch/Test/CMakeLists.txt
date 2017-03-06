cmake_minimum_required (VERSION 2.6)
project (DFGPresetSearchWidget)

set(
  FabricStageDir "${FABRIC_DIR}"
  CACHE FILEPATH "desc" )

set(
  QtDir "C:/Qt5/5.7/msvc2013_64"
  CACHE FILEPATH "desc" )

add_definitions(-DFEC_SHARED)

add_custom_target(
  qtMoc
  COMMAND "${CMAKE_CURRENT_LIST_DIR}/moc.bat" ${QtDir} ${CMAKE_CURRENT_LIST_DIR}
)

include_directories(
  ../
  ${FabricStageDir}/include
  ${FabricStageDir}/include/FabricServices
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
  ../QueryEdit.cpp
  ../ResultsView.cpp
  ../ResultPreview.cpp
  ../ItemView.cpp
  ../HeatBar.cpp
  Moc.cpp
  Main.cpp
  )

file( COPY ${CMAKE_CURRENT_LIST_DIR}/TabSearch.qss DESTINATION ${CMAKE_BINARY_DIR}/ )

add_dependencies( DFGPresetSearchWidget qtMoc )

target_link_libraries(
  DFGPresetSearchWidget
  FabricCore-2.5.lib
  Qt5Gui.lib
  Qt5Widgets.lib
  Qt5Core.lib
  )
