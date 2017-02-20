cmake_minimum_required (VERSION 2.6)
project (DFGTabSearchWidget)

set(
  FabricStageDir "${FABRIC_DIR}"
  CACHE FILEPATH "desc" )

set(
  QtDir "C:/Qt5/5.7/msvc2013_64"
  CACHE FILEPATH "desc" )

add_definitions(-DFEC_SHARED)

execute_process( COMMAND "${CMAKE_CURRENT_LIST_DIR}/moc.bat" ${QtDir} ${CMAKE_CURRENT_LIST_DIR} )

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
  DFGTabSearchWidget
  ../DFGTabSearchWidget.cpp
  Moc.cpp
  Main.cpp
  )

target_link_libraries(
  DFGTabSearchWidget
  FabricCore-2.5.lib
  Qt5Gui.lib
  Qt5Widgets.lib
  Qt5Core.lib
  )
