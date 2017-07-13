set( qssIn ${SGDir}/Native/FabricUI/FabricUI.qss )
set( qssOut ${CMAKE_BINARY_DIR}/FabricUI.qss )
add_custom_target(
  qss
  COMMAND ${CMAKE_COMMAND} -E copy ${qssIn} ${qssOut}
)

file( GLOB Tests "${CMAKE_CURRENT_LIST_DIR}/*Test.cpp" )
foreach( Test ${Tests} )
  get_filename_component( TestName "${Test}" NAME_WE )
  add_executable( ${TestName} ${Test} )
  target_link_libraries( ${TestName} PRIVATE FCurveEditor )
  add_dependencies( ${TestName} qss )
endforeach()
target_link_libraries( FCurveEditorTest PRIVATE AnimXFCurveModel )
target_link_libraries( RTValFCurveEditorTest PRIVATE RTValAnimXFCurveModel )

if( NOT DEFINED FABRIC_DIR )
  set( FABRIC_DIR "${SGDir}/stage/${CMAKE_SYSTEM_NAME}/Release" )
endif()
