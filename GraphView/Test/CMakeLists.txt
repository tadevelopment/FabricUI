cmake_minimum_required (VERSION 2.6)
project (GraphView)

set(
  FabricStageDir "${FABRIC_DIR}"
  CACHE PATH "desc" )

set(
  QtDir ""
  CACHE PATH "desc" )

if( FabricStageDir STREQUAL "" )
  message(SEND_ERROR "Please specify FabricStageDir (where bin/ is located)" )
endif()

if( QtDir STREQUAL "" )
  message(SEND_ERROR "Please specify QtDir (where bin/ is located)" )
endif()

add_definitions(-DFEC_SHARED)
add_definitions(-DFABRIC_OS_WINDOWS)

set( SGDir "../../../.." )

file( WRITE ${CMAKE_BINARY_DIR}/Moc.cpp "" )

add_custom_target(
  qtMoc
  COMMAND "${CMAKE_CURRENT_LIST_DIR}/moc.bat" ${QtDir} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR}/${SGDir}
)

include_directories(
  ${SGDir}
  ${SGDir}/Native
  ${SGDir}/Core/Clients/CAPI
  ${SGDir}/Native/FabricServices
  ${QtDir}/include ${QtDir}/include/QtCore
  ${QtDir}/include/QtGUI
  ${QtDir}/include/ActiveQt
  ${QtDir}/include/QtWidgets
  ${QtDir}/include/QtOpenGL
  )

link_directories(
  ${FabricStageDir}/lib
  ${QtDir}/lib
  )

file(
  GLOB TabSearchSrc
  "${SGDir}/Native/FabricUI/DFG/TabSearch/*.cpp"
  "${SGDir}/Native/FabricUI/DFG/TabSearch/*.h"
)

file(
  GLOB GraphViewSrc
  "${SGDir}/Native/FabricUI/GraphView/*.cpp"
  "${SGDir}/Native/FabricUI/GraphView/*.h"
)

file(
  GLOB FSCommands
  "${SGDir}/Native/FabricServices/Commands/*.cpp"
  "${SGDir}/Native/FabricServices/Commands/*.h"
)

file(
  GLOB FabricUI_Util
  "${SGDir}/Native/FabricUI/Util/*.cpp"
  "${SGDir}/Native/FabricUI/GraphView/*.h"
)

add_executable(
  GraphView
  ${TabSearchSrc}
  ${GraphViewSrc}
  ${FabricUI_Util}
  ${FSCommands}
  ${SGDir}/Native/FabricUI/Style/FabricStyle.cpp
  ${SGDir}/Native/FabricUI/GraphView/Test/Main.cpp
  ${SGDir}/Native/FabricUI/GraphView/Test/Main.h
  ${CMAKE_BINARY_DIR}/Moc.cpp
  )

source_group(TabSearch FILES ${TabSearchSrc})
source_group(GraphView FILES ${GraphViewSrc})

configure_file( ${SGDir}/Native/FabricUI/FabricUI.qss ${CMAKE_BINARY_DIR}/ )

add_dependencies( GraphView qtMoc )

target_link_libraries(
  GraphView
  FabricCore-2.6.lib
  Qt5Gui.lib
  Qt5Widgets.lib
  Qt5Core.lib
  Qt5OpenGL.lib
  )
