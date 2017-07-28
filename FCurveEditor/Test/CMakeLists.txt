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
target_link_libraries( 02_FCurveEditorTest PRIVATE AnimXFCurveModel )
target_link_libraries( 03_RTValFCurveEditorTest PRIVATE RTValAnimXFCurveModel )
target_link_libraries( 04_RTValFCurveUndoRedoTest PRIVATE RTValAnimXFCurveDFGController )

if( MSVC_VERSION )
  message( STATUS
    "You might need to add the following definitions to "
    "Visual Studio's debug environment :\n"
    "PATH=${QtDir}/bin;${FabricStageDir}/bin;%PATH%\n"
    "FABRIC_EXTS_PATH=${FabricStageDir}/Exts\n"
    "FABRIC_DIR=${FabricStageDir}"
  )
endif()
