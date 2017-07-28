set( CommandDir  ${SGDir}/Native/FabricUI/Commands )

file( GLOB CommandHeaders "${CommandDir}/*.h" )
file( GLOB CommandCpps "${CommandDir}/*.cpp" )

add_library( FabricUI
  ${SGDir}/Native/FabricUI/Application/FabricException.cpp
  ${SGDir}/Native/FabricUI/Application/FabricApplicationStates.cpp
  ${SGDir}/Native/FabricUI/Util/Factory.cpp
  ${SGDir}/Native/FabricUI/Util/RTValUtil.cpp
  ${SGDir}/Native/FabricUI/DFG/Commands/DFGPathValueResolver.cpp
  ${CommandCpps}
)
set( Headers
  ${CommandHeaders}
  ${SGDir}/Native/FabricUI/Application/FabricException.h
  ${SGDir}/Native/FabricUI/Application/FabricApplicationStates.h
  ${SGDir}/Native/FabricUI/Util/Factory.h
  ${SGDir}/Native/FabricUI/Util/RTValUtil.h
  ${SGDir}/Native/FabricUI/DFG/Commands/DFGPathValueResolver.h
)
QtMocProcess( "${Headers}" FabricUI )

target_include_directories( FabricUI PUBLIC
  ${SGDir}/Native/FabricServices
)
target_link_libraries( FabricUI PUBLIC
  RTValAnimXFCurveModel
)

add_library( RTValAnimXFCurveDFGController
  ${CMAKE_CURRENT_LIST_DIR}/RTValAnimXFCurveDFGController.cpp
)
QtMocProcess( "${CMAKE_CURRENT_LIST_DIR}/RTValAnimXFCurveDFGController.h" RTValAnimXFCurveDFGController )

target_link_libraries( RTValAnimXFCurveDFGController PUBLIC
  RTValAnimXFCurveModel
  FabricUI
)
