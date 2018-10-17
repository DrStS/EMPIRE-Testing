#------------------------------------------------------------------------------------#
file(GLOB SOURCES "*.cpp" "*.c")
#------------------------------------------------------------------------------------#
include_directories($ENV{EMPIRE_API_INC_ON_MACHINE})
#------------------------------------------------------------------------------------#
#SET(Emperor_MapperTest_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}" )
#------------------------------------------------------------------------------------#
#get_property(Emperor_INCLUDES GLOBAL PROPERTY Emperor_INCLUDES)
get_property(EMPIRE_thirdparty_INCLUDES GLOBAL PROPERTY EMPIRE_thirdparty_INCLUDES) 
#------------------------------------------------------------------------------------#
#include_directories(${Emperor_INCLUDES})
include_directories(${EMPIRE_thirdparty_INCLUDES})
#------------------------------------------------------------------------------------#
add_executable(optimizationClient ${SOURCES})
target_link_libraries(optimizationClient EmperorLib $ENV{EMPIRE_API_LIB_ON_MACHINE})
target_link_libraries(optimizationClient ${INSTALL_DIR}/libnlopt.a)
#------------------------------------------------------------------------------------#
add_dependencies(optimizationClient EMPIRE_API_Static EMPIRE_API_Shared)